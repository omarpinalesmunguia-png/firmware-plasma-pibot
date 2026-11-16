# Configuracion minima PiBot Plasma

La limpieza se realiza con opciones de compilacion. No se elimina codigo del
nucleo grblHAL ni de los submodulos oficiales.

## Activo en la Etapa 2

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

La Etapa 2 autoriza pruebas controladas de corte a altura fija una vez
verificados en seco M3/M5, la sonda y la entrada Arc OK.

## Presente pero apagado hasta las etapas siguientes

| Funcion | Estado | Condicion para habilitarla |
|---|---:|---|
| Plugin Plasma/THC completo | Limitado a Arc OK | La parte que controla Z permanece excluida |
| Arc OK para el plugin | Activo | GPIO39, puerto auxiliar logico 7 |
| MCP3221 / lectura de voltaje | Apagado | Etapa 3: calibrarlo inicialmente sin permitir movimientos Z |
| Correccion automatica Z | Apagada | Etapa 4: habilitarla solo despues de validar altura fija y voltaje |

La bandera `PIBOT_PLASMA_PLUGIN_ENABLE` esta activa. Las banderas
`PIBOT_PLASMA_FIXED_HEIGHT_ONLY=1` y `PIBOT_ARC_VOLTAGE_ENABLE=0` impiden que
el plugin tome control de Z o use el convertidor de voltaje.

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

## Criterio para avanzar a la etapa de monitoreo de voltaje

Antes de habilitar el monitoreo de voltaje, el firmware de Etapa 2 debe:

1. Encender sin alarma inesperada.
2. Mover X/Y/Z en el sentido correcto y conservar posicion.
3. Sondear y repetir Z0 sin deriva.
4. Activar y desactivar en seco la salida M3/M5.
5. Confirmar que ningun movimiento modifica Z sin una orden explicita.

La siguiente etapa habilitara el MCP3221 solamente como monitor. No se
habilitara correccion Z hasta repetir y aprobar varios cortes a altura fija.
