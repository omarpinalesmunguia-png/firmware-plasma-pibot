# Integracion minima del plugin oficial Plasma/THC.
#
# Este archivo usa el punto de extension provisto por el controlador ESP32 y
# evita modificar su CMakeLists.txt original. PLASMA_ENABLE se habilitara mas
# adelante, cuando se agregue y valide la configuracion de la PiBot 5.88 Ultra.

list(APPEND SRCS
 grbl/pid.c
 plasma/thc.c
 plasma/linuxcnc.c
 plasma/sheetcam.c
)
