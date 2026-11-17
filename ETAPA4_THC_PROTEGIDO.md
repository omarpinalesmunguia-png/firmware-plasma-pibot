# Etapa 4: THC protegido

Esta etapa habilita por primera vez la correccion automatica de Z sobre la
base limpia. La correccion no queda activa por el solo hecho de encender la
antorcha: requiere el protocolo `THC:PREPARE` / `THC:READY` de la aplicacion.
El boton de altura fija mantiene Z fuera del control del plugin.

## Protecciones

- Objetivo enviado por la aplicacion para cada contorno.
- Retardo de 1.5 s despues de `READY`.
- Banda muerta de +/-3 V.
- Correccion solamente por encima del 90% del avance programado.
- Cinco muestras y filtro de mediana antes de corregir.
- Un salto mayor de 15 V respecto a la mediana no entra al control.
- Un error mayor de 20 V respecto al objetivo bloquea la correccion.
- Un fallo del MCP3221 bloquea Z, pero no pausa el corte ni la red.
- Cada orden Z es de 0.03 mm como maximo.
- Velocidad Z limitada a 30 mm/min y al 5% del avance XY.
- Recorrido limitado a +/-0.50 mm respecto de la altura donde inicio el THC.
- Altura absoluta minima de 0.80 mm.
- ArcOK perdido detiene primero Z y respeta la gracia enviada por la
  aplicacion antes de apagar y pausar.
- `LEADOUT`, `M5`, reset y fin de trabajo liberan el motor Z y sincronizan su
  posicion con grblHAL.

## Configuracion de la primera prueba

- Placa ya validada con aire estable.
- Linea recta de 100 mm.
- Altura inicial: 1.30 mm.
- Avance: F400.
- Objetivo inicial: 104 V.
- Primero repetir una linea con el boton de altura fija.
- Despues ejecutar una sola linea con el boton `Con THC activo`.

Durante la prueba THC, Z debe permanecer entre 0.80 y 1.80 mm. Las letras `U`
y `D` del campo THC indican correccion; `R` indica regulacion habilitada. Un
pico aislado, como el de 148.5 V observado en la Etapa 3.1, debe bloquear esa
muestra sin producir un salto Z.

No se autorizan todavia contornos complejos, cruces de ranuras ni produccion.
La primera validacion debe hacerse en una linea recta con acceso inmediato a
Feed Hold y E-stop.
