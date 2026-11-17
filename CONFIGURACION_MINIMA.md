# Configuracion minima PiBot Plasma

La limpieza se realiza con opciones de compilacion. No se elimina codigo del
nucleo grblHAL ni de los submodulos oficiales.

## Activo en la Etapa 3

| Funcion | Estado | Motivo |
|---|---:|---|
| ESP32-S3 / PiBot 5.88 Ultra | Activa | Plataforma de la controladora |
| Ejes X, Y y Z | Activos | Movimiento de la mesa y altura |
| Antorcha digital M3/M5 | Activa para prueba seca | Reutiliza el control spindle solo como interruptor |
| Sonda / cabezal flotante | Activa | Referencia Z antes del corte |
| E-stop, Feed Hold, Cycle Start | Activos | Controles fisicos de seguridad |
| WiFi SoftAP y Telnet | Activos | Conexion con la aplicacion de escritorio |
| Plugin Plasma | Arc OK solamente | Supervisa encendido y perdida de arco sin controlar Z |
| Corte a altura fija | Activo para prueba | La aplicacion ordena toda posicion Z |
| MCP3221 / lectura de voltaje | Activo como monitor | Mide y reporta sin corregir Z |

La Etapa 3 autoriza pruebas controladas de telemetria durante cortes a altura
fija. Todavia no autoriza correccion automatica de altura.

## Presente pero apagado hasta las etapas siguientes

| Funcion | Estado | Condicion para habilitarla |
|---|---:|---|
| Plugin Plasma/THC completo | Limitado a Arc OK | La parte que controla Z permanece excluida |
| Arc OK para el plugin | Activo | GPIO39, puerto auxiliar logico 7 |
| Correccion automatica Z | Apagada | Etapa 4: habilitarla solo despues de validar altura fija y voltaje |

La bandera `PIBOT_PLASMA_PLUGIN_ENABLE` esta activa. Las banderas
`PIBOT_PLASMA_FIXED_HEIGHT_ONLY=1`, `PIBOT_ARC_VOLTAGE_ENABLE=1` y
`PIBOT_VOLTAGE_MONITOR_ONLY=1` permiten medir el voltaje, pero impiden que el
plugin tome control de Z.

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
| MCP3221 SDA activo | GPIO14 |
| MCP3221 SCL activo | GPIO13 |

## Criterio para avanzar a la etapa de correccion de altura

Antes de habilitar cualquier correccion Z, el firmware de Etapa 3 debe:

1. Encender sin alarma inesperada.
2. Mover X/Y/Z en el sentido correcto y conservar posicion.
3. Sondear y repetir Z0 sin deriva.
4. Activar y desactivar en seco la salida M3/M5.
5. Confirmar que ningun movimiento modifica Z sin una orden explicita.
6. Registrar voltaje en reposo, perforacion, corte estable y salida de arco.
7. Validar la escala `$361` y el offset `$362` con una referencia confiable.

No se habilitara correccion Z hasta repetir y aprobar varios cortes a altura
fija con telemetria consistente.
