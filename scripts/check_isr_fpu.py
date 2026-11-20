#!/usr/bin/env python3
"""
Comprueba que ninguna rutina que pueda ejecutarse en contexto de interrupcion
contenga instrucciones de coma flotante (FPU) del ESP32-S3.

Motivo: en el ESP32-S3 la FPU no esta disponible dentro de una ISR. Si la tarea
interrumpida no tiene contexto FPU (p. ej. la tarea IDLE) se produce una
"Coprocessor exception" (EXCCAUSE 4) y el chip se reinicia; si la tiene, la ISR
corrompe en silencio los registros FPU de esa tarea.

Reglas (todas las violaciones hacen fallar la compilacion, exit code 1):
  V1  Funcion propia del proyecto ubicada en IRAM (IRAM_ATTR / ISR_CODE) con
      instrucciones FPU o PIE, salvo las de la lista permitida.
  V2  Cualquier funcion con instrucciones FPU/PIE alcanzable por llamadas
      directas desde una raiz de ISR (funciones propias en IRAM + ganchos
      conocidos de hal.*), salvo las de la lista permitida.
  V3  (solo aviso) Codigo propio en flash alcanzable desde una raiz de ISR:
      falla si la cache de flash esta desactivada (p. ej. escritura en NVS).

Limitacion: las llamadas indirectas (punteros a funcion) no se resuelven desde
el binario; por eso los ganchos de hal.* se listan por nombre en ISR_HOOKS.
Si se anade un nuevo gancho que corra en ISR, anadirlo a esa lista.

Uso:  check_isr_fpu.py build/grbl.elf [--report build/isr_fpu_report.txt]
                                      [--allow nombre ...]
"""
import argparse
import bisect
import re
import subprocess
import sys

# Funciones con FPU que viven en IRAM pero se ejecutan solo en primer plano.
DEFAULT_ALLOW = {
    "wait_on_input",  # ioports.c: espera M66 desde el bucle principal; IRAM_ATTR solo por velocidad
}

# Raices por nombre: ISR y ganchos hal.* invocados desde ISR (pueden estar en flash
# si alguien olvida ISR_CODE, por eso se buscan aunque no esten en IRAM).
ISR_HOOKS = [
    "stepper_driver_isr", "stepper_driver_interrupt_handler",
    "gpio_isr", "gpio_limit_isr", "gpio_control_isr", "gpio_aux_isr",
    "gpio_mpg_isr", "gpio_i2c_strobe_isr",
    "limit_interrupt_handler", "control_interrupt_handler", "aux_irq_handler",
    "ioports_event", "motor_irq",
    "timer01_isr", "timer10_isr", "timer11_isr",
    "stepperPulseStart", "I2SStepperPulseStart", "stepperGoIdle",
    "I2SStepperGoIdle", "stepperCyclesPerTick", "I2SStepperCyclesPerTick",
    "stepperOutputStep", "I2SStepperOutputStep",
    "st_spindle_sync_out", "stepperPulseStartPPI", "dynamic_current_pulse_start",
    "onStepperPulse", "trap_control_interrupts",
]
# Raices que deben existir para dar por valido el analisis (evita falsos "OK").
MUST_EXIST = ["stepper_driver_isr", "stepper_driver_interrupt_handler", "stepperPulseStart"]

FPU = set("""add.s sub.s mul.s madd.s msub.s neg.s abs.s mov.s float.s ufloat.s trunc.s
utrunc.s round.s floor.s ceil.s oeq.s ole.s olt.s ueq.s ule.s ult.s un.s moveqz.s
movgez.s movltz.s movnez.s movf.s movt.s sqrt0.s recip0.s rsqrt0.s div0.s divn.s
nexp01.s addexp.s addexpm.s mkdadj.s mksadj.s const.s maddn.s
lsi lsiu lsx lsxu ssi ssiu ssx ssxu rfr wfr rur.fcr rur.fsr wur.fcr wur.fsr""".split())

HDR = re.compile(r"^([0-9a-f]{8}) <(.+)>:$")
INS = re.compile(r"^\s*([0-9a-f]+):\t(\S+)\s*(.*)$")
SEC = re.compile(r"^Disassembly of section (\S+):")
TGT = re.compile(r"^([0-9a-f]{8}) <(.+?)(\+0x[0-9a-f]+)?>")


def run(cmd):
    p = subprocess.run(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                       universal_newlines=True, errors="replace")
    if p.returncode != 0:
        sys.exit("ERROR ejecutando %s:\n%s" % (" ".join(cmd[:2]), p.stderr[:500]))
    return p.stdout


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("elf")
    ap.add_argument("--objdump", default="xtensa-esp32s3-elf-objdump")
    ap.add_argument("--addr2line", default="xtensa-esp32s3-elf-addr2line")
    ap.add_argument("--report", default=None)
    ap.add_argument("--allow", nargs="*", default=[])
    a = ap.parse_args()
    allow = set(DEFAULT_ALLOW) | set(a.allow)

    dis = run([a.objdump, "-d", "-C", "--no-show-raw-insn", a.elf]).splitlines()

    funcs, cur, cursec = {}, None, None
    for line in dis:
        m = SEC.match(line)
        if m:
            cursec = m.group(1)
            continue
        m = HDR.match(line)
        if m:
            addr = int(m.group(1), 16)
            cur = {"addr": addr, "name": m.group(2), "sec": cursec or "",
                   "fpu": {}, "pie": 0, "calls": set(), "end": addr}
            funcs[addr] = cur
            continue
        m = INS.match(line)
        if not (m and cur is not None):
            continue
        ia, mn, op = int(m.group(1), 16), m.group(2), m.group(3)
        cur["end"] = max(cur["end"], ia + 2)
        if mn in FPU:
            cur["fpu"][mn] = cur["fpu"].get(mn, 0) + 1
        elif mn.startswith("ee."):
            cur["pie"] += 1
        elif mn.startswith("call") and not mn.startswith("callx"):
            t = TGT.match(op)
            if t:
                cur["calls"].add(int(t.group(1), 16))
        elif mn in ("j", "j.l"):
            t = TGT.match(op)
            if t and t.group(2) != cur["name"]:
                cur["calls"].add(int(t.group(1), 16))

    if not funcs:
        sys.exit("ERROR: el desensamblado no contiene funciones (toolchain/ELF incorrectos)")

    starts = sorted(funcs)

    def owner(x):
        i = bisect.bisect_right(starts, x) - 1
        return starts[i] if i >= 0 else None

    for f in funcs.values():
        f["calls"] = {o for o in (owner(t) for t in f["calls"]) if o is not None}

    # Origen (archivo:linea) de cada funcion para distinguir codigo propio del de ESP-IDF
    src = {}
    for i in range(0, len(starts), 1500):
        chunk = [hex(x) for x in starts[i:i + 1500]]
        out = run([a.addr2line, "-e", a.elf] + chunk).splitlines()
        for x, l in zip(chunk, out):
            src[int(x, 16)] = l.strip()
    for x, f in funcs.items():
        f["src"] = src.get(x, "??")

    def mine(f):
        p = f["src"]
        return "/main/" in p and "/components/" not in p and "/opt/esp" not in p

    def is_iram(f):
        return f["sec"].startswith(".iram")

    def uses_fpu(f):
        return bool(f["fpu"]) or f["pie"] > 0

    by_name = {}
    for x, f in funcs.items():
        by_name.setdefault(f["name"], []).append(x)

    out_lines = []

    def log(s=""):
        out_lines.append(s)
        print(s)

    missing = [n for n in MUST_EXIST if n not in by_name]
    if missing:
        log("ERROR: faltan raices obligatorias en el ELF: %s" % ", ".join(missing))
        log("El analisis no es fiable; revisar nombres/compilacion. FALLA.")
        finish(out_lines, a.report)
        return 2

    roots = {x for x, f in funcs.items() if is_iram(f) and mine(f) and f["name"] not in allow}
    for n in ISR_HOOKS:
        for x in by_name.get(n, []):
            roots.add(x)

    # Alcanzabilidad por llamadas directas
    parent, stack = {}, [(r, None) for r in sorted(roots)]
    while stack:
        x, p = stack.pop()
        if x in parent:
            continue
        parent[x] = p
        for c in funcs[x]["calls"]:
            if c not in parent:
                stack.append((c, x))

    def chain(x):
        names = []
        while x is not None and len(names) < 8:
            names.append(funcs[x]["name"])
            x = parent[x]
        return " <- ".join(names)

    log("Auditoria FPU en ISR  |  ELF: %s" % a.elf)
    log("Funciones: %d | raices ISR: %d | alcanzables: %d | permitidas: %s"
        % (len(funcs), len(roots), len(parent), ", ".join(sorted(allow)) or "-"))
    log()

    violations, warnings = [], []
    for x in sorted(parent):
        f = funcs[x]
        if f["name"] in allow:
            continue
        if uses_fpu(f):
            kind = "V1 (IRAM propia)" if (is_iram(f) and mine(f)) else "V2 (alcanzable desde ISR)"
            violations.append((kind, x, f))
        elif f["sec"] == ".flash.text" and mine(f):
            warnings.append((x, f))
    # V1 tambien para funciones propias en IRAM no alcanzables desde raices (p. ej. todas son raices; red de seguridad)
    for x, f in funcs.items():
        if x not in parent and is_iram(f) and mine(f) and uses_fpu(f) and f["name"] not in allow:
            violations.append(("V1 (IRAM propia)", x, f))

    if violations:
        log("*** VIOLACIONES: instrucciones FPU/PIE en contexto de interrupcion ***")
        for kind, x, f in violations:
            log("[%s] %s @0x%08x [%s]" % (kind, f["name"], x, f["sec"]))
            log("    %s" % f["src"])
            log("    FPU: %s  PIE: %d" % (dict(f["fpu"]), f["pie"]))
            if x in parent:
                log("    cadena: %s" % chain(x))
        log()
    if warnings:
        log("Avisos (no bloquean): codigo propio en FLASH alcanzable desde ISR "
            "(riesgo solo con la cache de flash desactivada):")
        for x, f in warnings:
            log("  %s @0x%08x  %s  <- %s" % (f["name"], x, f["src"].replace("/github/workspace/", ""),
                                             chain(x).split(" <- ")[1] if " <- " in chain(x) else "raiz"))
        log()

    if violations:
        log("RESULTADO: FALLA (%d violaciones). La ISR no debe usar coma flotante: "
            "copiar bits crudos con enteros y calcular en primer plano." % len(violations))
        finish(out_lines, a.report)
        return 1
    log("RESULTADO: OK - ninguna funcion alcanzable desde ISR usa FPU/PIE.")
    finish(out_lines, a.report)
    return 0


def finish(lines, report):
    if report:
        with open(report, "w") as fh:
            fh.write("\n".join(lines) + "\n")


if __name__ == "__main__":
    sys.exit(main())
