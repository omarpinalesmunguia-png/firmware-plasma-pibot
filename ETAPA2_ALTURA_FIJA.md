# Etapa 2: corte a altura fija

Esta etapa agrega al firmware estable solamente la supervision de plasma que
se necesita para probar cortes a altura fija.

## Funciones activas

- Encendido y apagado de antorcha mediante M3/M5 en GPIO7.
- Entrada Arc OK en GPIO39, puerto auxiliar logico 7.
- M3 espera hasta 3 segundos la confirmacion Arc OK.
- Si el arco se pierde durante el corte, la tarjeta apaga la antorcha y entra
  en pausa.
- Estado `THC` reportado para que la aplicacion muestre antorcha y Arc OK.
- WiFi, reconexion, movimientos, sonda y controles de la Etapa 1 sin cambios.

## Bloqueos intencionales

- MCP3221 y lectura de voltaje apagados.
- Regulacion THC por voltaje apagada.
- Inyeccion secundaria de pasos apagada porque no es compatible con el I2S de
  la PiBot 5.88 Ultra.
- El plugin no inicializa un segundo controlador de Z, no reclama el motor Z y
  no puede modificar su posicion.
- La altura de perforacion y la altura de corte siguen bajo control exclusivo
  de los comandos G-code enviados por la aplicacion.

## Identificacion esperada

Despues de cargar el firmware:

- `$350` debe mostrar modo 3 (Arc OK).
- `$367` debe mostrar puerto 7.
- `$I` debe incluir `PLASMA (Arc ok)`.
- `$PINS` debe identificar GPIO39 como la entrada Arc OK reclamada.

## Orden de pruebas

1. Con la fuente de plasma apagada, repetir movimientos X/Y/Z, sondeo y M3/M5.
2. Confirmar que diez movimientos Z ordenados terminan en la altura esperada.
3. Encender la fuente y probar M3/M5 con la antorcha lejos del material.
4. Confirmar las letras `T` (antorcha) y `A` (Arc OK) en el reporte THC.
5. Cortar una linea recta corta a altura fija, con THC deshabilitado.
6. Comparar la altura Z reportada antes, durante y despues del corte.

No se debe probar el boton de corte con THC en esta etapa.
