# Etapa 5E: limite acotado opcional del THC (`L=`)

Parche: `patches/pibot-plasma-etapa5e-thc-limite-acotado.patch` (se aplica despues de la 5D).

## Comando

```
(THC:PREPARE,G=<ms>,V=<volts>[,L=<mm>])
```

- `L=` es opcional. Sin `L=` el comportamiento es identico al anterior (limite de
  `PLASMA_THC_MAX_OFFSET_MM`, hoy 10 mm, y piso `PLASMA_THC_Z_MIN_MM`, hoy -10 mm).
- `L` solo puede **estrechar** el limite: rango 0.5 a 6.0 mm (`PLASMA_THC_LIMIT_MIN_MM`,
  `PLASMA_THC_LIMIT_MAX_MM`), y nunca mayor que `PLASMA_THC_MAX_OFFSET_MM`.
- Fuera de rango, no numerico, texto sobrante o `L=` sin `V=`: `GcodeValueOutOfRange` y el
  limite vigente **no cambia** (el limite solo se modifica tras validar todo el comando).
- `PREPARE` sin `V=` sigue rechazandose como antes (`SettingDisabled`); no se toco.

## Efecto

El THC se mueve en `[thc_start_z - L, thc_start_z + L]`, donde `thc_start_z` es la Z de trabajo
cuando el THC se activa (altura de corte tras la perforacion). Si el trabajo envia `L=`, ademas
se aplica un piso absoluto `PLASMA_THC_LIMITED_Z_MIN_MM` (0.80 mm) sobre el cero Z de trabajo.

Ejemplo: altura 1.5 mm y `L=6` -> limites [0.8, 7.5] (el piso evita -4.5 mm bajo la lamina).

## Restablecimiento

El limite vuelve al valor de la maquina en `reset()`, al apagar la antorcha y en cualquier
`PREPARE` sin `L=`, para que un limite no pase al trabajo siguiente.

## Suposiciones a confirmar

- El piso de 0.80 mm supone que el cero Z de trabajo es la superficie de la lamina (sondeo
  `G38.2` + `G10 L20 P0 Z0`). En mapa de alturas con lamina deformada hay que revisarlo.
- La aplicacion decide cuando enviar `L=`; sin `L=` no hay cambio de comportamiento.

## Prueba sugerida

1. Trabajo sin `L=`: debe comportarse igual que la 5D.
2. Corte de prueba con `L=1.0`: la Z del THC no debe salir de +-1 mm alrededor de la altura de corte.
3. Abortar con `L=` activo y lanzar un trabajo sin `L=`: debe volver al limite normal.
