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
- El plugin permanece deshabilitado hasta definir `PLASMA_ENABLE` y comprobar
  la asignacion de pines y entradas analogicas.

## Siguiente etapa

Crear una configuracion PiBot minima y auditable:

1. Seleccionar ESP32-S3 y el mapa original de la PiBot 5.88 Ultra.
2. Verificar fisicamente cada pin antes de agregar un mapa de plasma.
3. Habilitar antorcha, Arc OK, sonda flotante, I2C y MCP3221.
4. Compilar primero sin THC activo.
5. Probar movimientos y corte a altura fija.
6. Habilitar despues el control THC oficial sin comandos personalizados.

## Obtener exactamente esta base

Despues de clonar este repositorio se deben inicializar los componentes con:

```text
git submodule update --init --recursive
```

No se debe actualizar ningun submodulo a la ultima revision sin repetir las
pruebas de compatibilidad y registrar el nuevo identificador.
