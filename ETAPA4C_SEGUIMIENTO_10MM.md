# Etapa 4C: seguimiento de placa inclinada 10 mm

Esta etapa amplia el seguimiento THC para validar una placa con una diferencia
de altura de 10 mm a lo largo de una linea de 100 mm.

## Cambios

- Recorrido THC de +/-10 mm respecto de la altura donde inicia la regulacion.
- Piso de coordenada Z ampliado a -10 mm; Z0 sigue siendo el punto sondeado al
  inicio y no representa toda la superficie inclinada.
- Correccion configurada al 12% del avance XY.
- Velocidad maxima de correccion Z: 50 mm/min.
- Pasos de correccion conservados en 0.03 mm.
- Banda muerta conservada en +/-3 V.
- Si el cabezal flotante permanece activo cinco muestras consecutivas durante
  el arco, el firmware detiene Z, apaga la antorcha y entra en pausa.

## Prueba

1. Inclinar la placa exactamente 10.0 mm en el sentido X a lo largo de 100 mm.
2. Colocar el extremo bajo en X100 y el alto en X0.
3. Usar una linea X100, F400, altura inicial 2.4 mm y objetivo 105 V.
4. Mantener la presion dinamica recomendada durante todo el corte.
5. Tener Feed Hold y E-stop disponibles.

Si la placa desciende 10 mm, la referencia ideal de Z es:

| X recorrida | Z esperada |
|---:|---:|
| 0 mm | 2.4 mm |
| 10 mm | 1.4 mm |
| 20 mm | 0.4 mm |
| 30 mm | -0.6 mm |
| 40 mm | -1.6 mm |
| 50 mm | -2.6 mm |
| 60 mm | -3.6 mm |
| 70 mm | -4.6 mm |
| 80 mm | -5.6 mm |
| 90 mm | -6.6 mm |
| 100 mm | -7.6 mm |

La comparacion debe usar la distancia recorrida desde el inicio real del corte,
no la coordenada absoluta X. La lectura puede retrasarse durante los primeros
milimetros por los 1.5 s de estabilizacion y el filtro de cinco muestras.
