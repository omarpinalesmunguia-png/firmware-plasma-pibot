# Configuracion minima PiBot Plasma

La limpieza se realiza con opciones de compilacion. No se elimina codigo del
nucleo grblHAL ni de los submodulos oficiales.

## Activo en la Etapa 4C

| Funcion | Estado | Motivo |
|---|---:|---|
| ESP32-S3 / PiBot 5.88 Ultra | Activa | Plataforma de la controladora |
| Ejes X, Y y Z | Activos | Movimiento de la mesa y altura |
| Antorcha digital M3/M5 | Activa para prueba seca | Reutiliza el control spindle solo como interruptor |
| Sonda / cabezal flotante | Activa | Referencia Z antes del corte |
| E-stop, Feed Hold, Cycle Start | Activos | Controles fisicos de seguridad |
| WiFi SoftAP y Telnet | Activos | Conexion con la aplicacion de escritorio |
| Plugin Plasma | Voltaje protegido | Solo controla Z tras PREPARE/READY |
| Corte a altura fija | Activo y aislado | Sin PREPARE el plugin no puede reclamar Z |
| MCP3221 / lectura de voltaje | Monitor tolerante a fallos | Limita cada espera I2C a 20 ms y bloquea Z ante fallo |
| Correccion automatica Z | Seguimiento ampliado en prueba | +/-10 mm, pasos de 0.03 mm y maximo 50 mm/min |

La validacion de movimiento I2S en seco de la Etapa 4A y la primera regulacion
de la Etapa 4B quedaron aprobadas. La Etapa 4C valida una pendiente conocida de
10 mm en 100 mm e incorpora paro por activacion del cabezal flotante.

## Limitado hasta las etapas siguientes

| Funcion | Estado | Condicion para habilitarla |
|---|---:|---|
| Plugin Plasma/THC completo | Activo con limites | Falta validar contornos, esquinas y cruces |
| Arc OK para el plugin | Activo | GPIO39, puerto auxiliar logico 7 |
| Correccion automatica Z | Opt-in | Solo PREPARE/READY; LEADOUT y M5 la desactivan |

Las banderas `PIBOT_PLASMA_PLUGIN_ENABLE`, `PIBOT_ARC_VOLTAGE_ENABLE` y
`PIBOT_PROTECTED_THC_ENABLE` estan activas. `PIBOT_PLASMA_FIXED_HEIGHT_ONLY=0`
permite la inyeccion secundaria de pasos, pero el protocolo de la aplicacion y
los limites compilados mantienen el control Z deshabilitado por defecto.

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

## Criterio para avanzar a contornos con THC

Antes de probar contornos, el firmware de Etapa 4 debe:

1. Encender sin alarma inesperada y aprobar la prueba seca I2S 4A.
2. Mover X/Y/Z en el sentido correcto y conservar posicion a varias velocidades.
3. Sondear y repetir Z0 sin deriva.
4. Activar y desactivar en seco la salida M3/M5.
5. Confirmar que ningun movimiento modifica Z sin una orden explicita.
6. Registrar voltaje en reposo, perforacion, corte estable y salida de arco.
7. Validar la escala `$361` y el offset `$362` con una referencia confiable.
8. Confirmar que un fallo del MCP3221 bloquea la correccion sin afectar WiFi,
   XY ni Arc OK.
9. Confirmar en una linea que Z nunca sale de 0.80 a 1.80 mm.
10. Confirmar que un pico aislado de voltaje no produce movimiento Z.
