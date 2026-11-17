# Etapa 3.1: telemetria robusta sin control Z

Esta revision corrige el bloqueo observado al encender el arco durante la
primera prueba de la Etapa 3. El corte continua siendo de altura fija.

## Cambios frente a la Etapa 3

- El tiempo maximo de una lectura I2C baja de 1000 ms a 20 ms.
- El MCP3221 se muestrea a 25 Hz en lugar de 100 Hz.
- Una lectura I2C fallida se entrega al plugin como fallo explicito.
- Tres muestras fallidas o menores de 20 V con Arc OK activo marcan el voltaje
  como no valido (`THC:-1.0`).
- Despues de un fallo se deja descansar el bus durante un segundo y se vuelve
  a intentar automaticamente.
- Una lectura valida recupera la telemetria sin reiniciar la tarjeta.

El fallo del ADC no genera pausa, reset, cambio de velocidad ni movimiento Z.
Arc OK conserva su supervision independiente.

## Protecciones que permanecen

- `$350=3`: modo Arc OK.
- Inyeccion secundaria de pasos desactivada.
- Motor secundario THC sin inicializar.
- El plugin no reclama el eje Z.
- Escala inicial `$361=0.04180`, offset `$362=0` y puerto `$366=0`.

## Prueba recomendada

1. Cargar `grbl_pibot_telemetria_robusta_etapa3_1_merged_0x0.bin`.
2. Confirmar en reposo que el voltaje esta cerca de cero.
3. Hacer una linea de 100 mm a F400, altura fija 1.3 mm y THC deshabilitado.
4. Confirmar que Z permanece en 1.30 mm y que el trabajo termina aunque la
   lectura analogica falle.
5. Si aparece `THC:-1.0`, conservar el log: debe recuperarse posteriormente sin
   perder WiFi ni requerir reinicio.

No se debe activar todavia la correccion automatica de altura.
