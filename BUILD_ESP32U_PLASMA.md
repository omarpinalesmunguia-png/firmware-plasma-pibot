# Plasma con ESP32-WROOM-32U (4 MB, sin TF)

Segunda máquina de plasma que comparte este firmware (incluido el THC v2) y el programa
`PlasmaPibotLimpio` con la PiBot V5.88 Ultra. El G-code se guarda en la flash interna
(LittleFS montado como raíz): la app lo sube por FTP a `/pibot_trabajo.nc` y lo ejecuta con `$F=`,
igual que en la PiBot.

## Compilar y flashear (ESP-IDF v4.4)

```bash
. $HOME/esp/esp-idf/export.sh
cd firmware-plasma-pibot
idf.py -B build_esp32u \
  -D SDKCONFIG=build_esp32u/sdkconfig \
  -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp32;sdkconfig.defaults.esp32u_plasma" \
  -D ESP32U_PLASMA=ON \
  set-target esp32
idf.py -B build_esp32u -D SDKCONFIG=build_esp32u/sdkconfig -D ESP32U_PLASMA=ON build
idf.py -B build_esp32u -p COM5 flash monitor
```

* No agregues `sdkconfig.defaults` a la lista: ESP-IDF cargaría `sdkconfig.defaults.esp32` al final
  y volvería a la tabla de particiones genérica (960 KB de almacenamiento en vez de ~1.9 MB).
* Usa una carpeta de compilación aparte (`build_esp32u`) para no mezclarla con la de la PiBot (ESP32-S3).
* La compilación de la PiBot no cambia (`idf.py build` con la configuración de siempre).
* Primer arranque: `$RST=*` para cargar valores por defecto. La memoria interna se formatea sola.

## Mapa de pines

| Función | GPIO | Notas |
|---|---|---|
| X paso / dir | 26 / 27 | |
| Y paso / dir | 25 / 14 | Alimenta los 2 drivers de Y (un canal de buffer por driver). Segundo motor: invertir una bobina |
| Z paso / dir | 33 / 2 | |
| Enable motores | 15 | |
| Antorcha (M3/M5) | 4 | Pull-down en la entrada del relé/SSR |
| Relé sensor óhmico | 23 | `M64 P1` / `M65 P1` (mismo número que en la PiBot) |
| Salidas libres | 12 (P0), 18 (P2), 19 (P3) | 18/19 reservadas para Y2 con escuadrado. 12 debe estar en bajo al arrancar |
| Cabezal flotante (probe) | 32 | |
| Paro de emergencia / Pausa / Inicio | 34 / 35 / 36 | Sólo entrada: pull-up externa 10 kΩ |
| Entrada óhmica dedicada (futuro) | 5 | Libre por ahora |
| Arc OK | 13 | Debe ser la última entrada auxiliar (el plugin la toma por defecto) |
| Final de carrera X / Y / Z arriba | 16 / 17 / 39 | 39 sólo entrada: pull-up externa |
| I2C MCP3221 SDA / SCL | 21 / 22 | Pull-ups 4.7 kΩ |

* Paso/dirección/enable a los drivers a través de un 74HCT245 (5 V).
* No usar el ADC interno del ESP32: el voltaje de arco lo lee el MCP3221.

## Ajustes iniciales sugeridos
* `$350=1` (THC por voltaje), `$674=2` (Sync Z; la app lo escribe en cada corte con THC).
* `$367` (puerto Arc OK) se asigna solo; verificar con `$pins`.
* `$21=0`, `$22=0` hasta cablear y probar los finales de carrera.
* `$70` debe incluir FTP (la app lo revisa y lo activa).
* En la app: misma configuración que la PiBot; sólo cambia la IP. La salida del relé óhmico es 1.

## Memoria
Partición de app 2 MB (el firmware ocupa ~0.86 MB) + almacenamiento 1980 KB (`partitions_esp32u_4m.csv`).
Archivos de 250 KB caben sobrados.
La flash sólo se escribe con la máquina en Idle (carga FTP y ajustes antes de `$F=`).
