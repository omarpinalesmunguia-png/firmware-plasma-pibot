# Integracion minima del plugin oficial Plasma/THC.
#
# Este archivo usa el punto de extension provisto por el controlador ESP32 y
# evita modificar su CMakeLists.txt original. En la Etapa 2 el plugin se compila
# en modo Arc OK y altura fija: no reclama ni mueve el eje Z.

list(APPEND SRCS
 grbl/pid.c
 plasma/thc.c
 plasma/linuxcnc.c
 plasma/sheetcam.c
)
