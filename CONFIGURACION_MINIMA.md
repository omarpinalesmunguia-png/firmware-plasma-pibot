# Configuracion minima PiBot Plasma

La limpieza se realiza con opciones de compilacion. No se elimina codigo del
nucleo grblHAL ni de los submodulos oficiales.

## Activo en la Etapa 1

| Funcion | Estado | Motivo |
|---|---:|---|
| ESP32-S3 / PiBot 5.88 Ultra | Activa | Plataforma de la controladora |
| Ejes X, Y y Z | Activos | Movimiento de la mesa y altura |
| Antorcha digital M3/M5 | Activa para prueba seca | Reutiliza el control spindle solo como interruptor |
| Sonda / cabezal flotante | Activa | Referencia Z antes del corte |
| E-stop, Feed Hold, Cycle Start | Activos | Controles fisicos de seguridad |
| WiFi SoftAP y Telnet | Activos | Conexion con la aplicacion de escritorio |

En esta etapa no se autorizan cortes reales porque Arc OK todavia no esta
supervisado. M3/M5 se prueba sin encender la fuente de plasma.

## Presente pero apagado hasta las etapas siguientes

| Funcion | Estado | Condicion para habilitarla |
|---|---:|---|
| Plugin Plasma/THC oficial | Apagado | Corregir primero el uso de punto flotante en la interrupcion ESP32-S3 |
| Arc OK para el plugin | Reservado | Etapa 2: validar polaridad y puerto 7 reportado por `$PINS` |
| MCP3221 / lectura de voltaje | Apagado | Etapa 3: calibrarlo inicialmente sin permitir movimientos Z |
| Correccion automatica Z | Apagada | Etapa 4: habilitarla solo despues de validar altura fija y voltaje |

Las banderas `PIBOT_PLASMA_PLUGIN_ENABLE` y
`PIBOT_ARC_VOLTAGE_ENABLE`, ubicadas en `main/my_machine.h`, deben permanecer
en `0` durante las primeras pruebas secas.

## Deshabilitado

| Funcion | Estado |
|---|---:|
| PWM y direccion de spindle | Deshabilitados |
| Refrigerante flood/mist | Deshabilitado |
| Laser y PPI | Deshabilitados |
| Bluetooth y Ethernet | Deshabilitados |
| WebUI, FTP y tarjeta SD | Deshabilitados |
| Modbus/VFD | Deshabilitado |
| Trinamic, MPG, teclado y macros | Deshabilitados |
| Ejes adicionales y autoescuadrado | Deshabilitados |
| OpenPnP, bordado y complementos auxiliares | Deshabilitados |

## Pines utilizados

| Funcion | Pin |
|---|---:|
| Sonda flotante | GPIO47 |
| E-stop | GPIO42 |
| Feed Hold | GPIO41 |
| Cycle Start | GPIO40 |
| Arc OK reservado, puerto logico 7 | GPIO39 |
| Limite Z | GPIO38 |
| Antorcha M3/M5 | GPIO7 |
| MCP3221 SDA reservado | GPIO14 |
| MCP3221 SCL reservado | GPIO13 |

## Criterio para avanzar a la Etapa 2

Antes de habilitar el plugin, el firmware de Etapa 1 debe:

1. Encender sin alarma inesperada.
2. Mover X/Y/Z en el sentido correcto y conservar posicion.
3. Sondear y repetir Z0 sin deriva.
4. Activar y desactivar en seco la salida M3/M5.
5. Confirmar que ningun movimiento modifica Z sin una orden explicita.

La Etapa 2 incorporara el plugin solamente como monitor Arc OK, junto con la
adaptacion que evita operaciones de punto flotante dentro de la interrupcion
de pasos del ESP32-S3. Solo entonces se autorizaran cortes a altura fija.
