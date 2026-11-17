# Etapa 3: telemetria THC sin control Z

Esta etapa conserva el corte a altura fija validado en la Etapa 2 y agrega
solamente la lectura del voltaje de arco mediante el MCP3221.

## Garantias de seguridad de esta etapa

- El modo Plasma permanece fijado en `$350=3` (Arc OK).
- El plugin no inicializa el motor secundario, no reclama Z y no instala los
  controladores que inyectan pasos de correccion.
- El valor del MCP3221 solo se publica en el campo `THC` del estado.
- La perdida de WiFi no detiene el trabajo almacenado en la tarjeta.
- Arc OK conserva la misma supervision de la Etapa 2.

Por lo anterior, activar o cambiar el voltaje objetivo en la aplicacion no
puede mover Z con este firmware.

## Entrada y conversion inicial

- MCP3221 I2C: direccion 0x4D.
- SDA: GPIO14.
- SCL: GPIO13.
- Puerto analogico del plugin: `$366=0`.
- Escala inicial: `$361=0.04180`.
- Offset inicial: `$362=0`.
- Muestreo: 100 lecturas por segundo; la aplicacion recibe el ultimo valor en
  cada reporte de estado.

La escala 0.04180 recupera el valor usado por el firmware anterior. Se debe
validar con el voltaje mostrado por la fuente o con una referencia segura. El
objetivo de esta etapa es medir y registrar; no corregir altura.

## Comprobaciones despues de cargar el binario

1. Con la fuente apagada, conectar por WiFi y consultar `$I`.
2. Consultar `$$` y confirmar `$350=3`, `$361=0.04180`, `$362=0`, `$366=0` y
   `$367=7`.
3. Confirmar que el voltaje en reposo es cercano a cero y no salta a valores
   extremos.
4. Repetir un sondeo y un movimiento corto de Z para comprobar que no cambio
   la calibracion de movimiento.
5. Hacer un corte recto de 100 mm a altura fija, con THC deshabilitado en la
   aplicacion, y compartir la bitacora completa.
6. Verificar fisicamente que Z permanece fijo durante todo el recorrido.

Para el primer corte no se debe usar el boton de corte con THC. Se busca medir
el voltaje estable, el pico de encendido y el comportamiento al finalizar el
arco antes de disenar la regulacion de la Etapa 4.

## Binario generado

El flujo publica el artefacto `pibot_telemetria_thc_etapa3` con el archivo:

`grbl_pibot_telemetria_thc_etapa3_merged_0x0.bin`
