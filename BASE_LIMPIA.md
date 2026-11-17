# Base limpia para PiBot 5.88 Ultra Plasma

Esta carpeta es el punto de partida limpio para reconstruir el firmware. No
contiene todavia los cambios de pines, ADC, WiFi ni control de trabajo de la
PiBot. Tampoco debe cargarse en la tarjeta hasta completar y verificar esas
adaptaciones.

## Revisiones fijadas

- Controlador ESP32 grblHAL: `0b2d3e1bb7da3b0ae0e03490b71275374eb84c67`
  (15 de octubre de 2026).
- Nucleo grblHAL: `4ec1ae44b857a70d2f9298344feac4b43ed0eb6b`
  (15 de octubre de 2026).
- Plugin oficial Plasma/THC: `b38b5fabfaa7ea6b871641c3e2d9612061afa202`
  (1 de noviembre de 2026).

El nucleo elegido es exactamente el que fue importado por la primera version
del repositorio anterior. El plugin elegido es la revision oficial posterior
que corrige un hard fault al usar puertos virtuales; es compatible con esa
interfaz del nucleo y precede al ultimo cambio, que solo actualiza el README.

## Estado de esta etapa

- El controlador ESP32 esta sin modificaciones PiBot.
- Todos los submodulos oficiales requeridos estan inicializados en sus
  revisiones fijadas.
- `main/plasma` es un submodulo del repositorio oficial, no una copia suelta.
- `3rdParty.cmake` agrega las fuentes oficiales del plugin usando el punto de
  extension previsto por el controlador ESP32.
- La rama de Etapa 4 conserva altura fija como modo seguro y agrega un THC
  opt-in para la primera prueba recta.
- El MCP3221 alimenta un filtro de mediana; fallos y picos bloquean la
  correccion sin detener XY ni WiFi.
- Las lecturas I2C tienen tiempo limitado, deteccion de fallo y reintento para
  que una perturbacion del plasma no bloquee estados ni WiFi.
- La regulacion THC queda limitada a +/-0.50 mm, pasos de 0.03 mm y 30 mm/min.

## Configuracion minima agregada

La rama conserva la configuracion validada de Etapa 1 para tres ejes, sonda,
controles fisicos, WiFi/Telnet y salida M3/M5. Agrega Arc OK y lectura de
voltaje y control Z protegido. Consulta `CONFIGURACION_MINIMA.md`,
`ETAPA2_ALTURA_FIJA.md`, `ETAPA3_TELEMETRIA_THC.md` y
`ETAPA3_1_TELEMETRIA_ROBUSTA.md` y `ETAPA4_THC_PROTEGIDO.md`.

La compilacion aplica ademas `patches/pibot-network-reconnect.patch` al
componente oficial de red. Esta correccion usa el keepalive corto que ya habia
funcionado en el firmware anterior para liberar una sesion Telnet abandonada,
sin cerrar sockets directamente desde el evento WiFi.

## Secuencia de validacion actual

Crear una configuracion PiBot minima y auditable:

1. Compilar para ESP32-S3 con THC protegido y MCP3221 robusto.
2. Verificar `$I`, `$350`, `$361`, `$362`, `$366`, `$367` y `$PINS`.
3. Repetir en seco movimientos, sondeo y M3/M5.
4. Repetir altura fija y confirmar que Z no cambia.
5. Probar una linea recta con objetivo 104 V y limites de Etapa 4.
6. Validar filtro, sentido de correccion y sincronizacion Z antes de contornos.

## Obtener exactamente esta base

Despues de clonar este repositorio se deben inicializar los componentes con:

```text
git submodule update --init --recursive
```

No se debe actualizar ningun submodulo a la ultima revision sin repetir las
pruebas de compatibilidad y registrar el nuevo identificador.
