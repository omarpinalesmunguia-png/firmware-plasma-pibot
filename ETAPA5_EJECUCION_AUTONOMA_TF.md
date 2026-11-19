# Etapa 5: ejecución autónoma desde tarjeta TF

Esta etapa conserva el seguimiento THC protegido de la Etapa 4C y corrige la
dependencia del enlace WiFi durante un trabajo.

## Flujo de trabajo

1. La aplicación genera el programa completo, incluidos sondeos, perforaciones,
   movimientos de corte, `M5` y retirada final.
2. El archivo se carga por FTP en `/sdcard/pibot_trabajo.nc`.
3. La aplicación ordena `$F=/sdcard/pibot_trabajo.nc`.
4. grblHAL lee y ejecuta el archivo localmente desde la TF.
5. WiFi/Telnet queda solamente para estado, progreso y gráfica.

La corrección `pibot-sd-autonomous-telnet.patch` evita que un cambio o pérdida
del flujo Telnet cierre el archivo activo. Al reconectar, los comandos de tiempo
real vuelven a estar disponibles sin reiniciar el programa de la TF.

## Modos cubiertos

- Altura fija con sondeo al iniciar cada contorno.
- THC protegido con `PREPARE`, `READY` y `LEADOUT` dentro del archivo.
- Mapa de alturas XYZ calculado previamente mediante sondeos múltiples.

## Prueba inicial obligatoria

La primera prueba debe hacerse con plasma apagado y mandos de antorcha
desconectados. Se ejecuta un recorrido largo desde TF, se desconecta el WiFi y
se verifica que la máquina termina el archivo, queda en `Idle` y efectúa la
retirada Z final. Después se repite un corte corto real.

## Binario

El flujo publica el artefacto `pibot_etapa5_autonomo_tf` con:

`grbl_pibot_etapa5_autonomo_tf_merged_0x0.bin`
