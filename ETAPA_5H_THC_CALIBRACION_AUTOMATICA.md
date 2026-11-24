# Etapa 5H: calibración automática del voltaje objetivo del THC

Se construye sobre 5G (que incluye 5D, 5E y 5F). `(THC:INFO)` responde **`THC control 4`**.

## Comando
```
(THC:PREPARE,G=<ms>,V=<voltaje manual>[,L=<mm>][,A=<ventana V>])
```
- Sin `A=` el THC funciona igual que en 5G.
- `A=` (3–30 V) activa la calibración automática para **esa figura**. `L=` y `A=` pueden ir en cualquier orden.

## Funcionamiento (por figura)
1. Después de `THC:READY` y del retardo del THC, el firmware **ignora los primeros 10 mm** de avance.
2. Espera a que el voltaje **se estabilice**. Usa las últimas 12 lecturas (~0.5 s a 25 Hz) y las toma como estables cuando:
   - el promedio de la mitad nueva y el de la mitad vieja difieren ≤ 2 V (sin tendencia), y
   - la desviación estándar es ≤ 5 V.

   Si no se estabiliza en 40 mm, usa el valor manual.
3. Promedia **30 mm** de avance a velocidad de corte. Descarta los picos que se alejan más de 12 V del nivel estable.
4. Si `|promedio − V manual| ≤ A`, el promedio pasa a ser el objetivo del THC. Si no, se queda el manual.

Durante la medición **la Z se mantiene** a la altura programada: el THC no corrige. Solo se mide con el THC activo, a ≥ `$356` % de la velocidad programada. Las esquinas lentas no cuentan, ni como distancia ni como lecturas.

## Mensajes (para que la app los guarde)
```
[MSG:THC AUTO V=111.3 REF=110.0 W=12 OK]
[MSG:THC AUTO V=124.0 REF=110.0 W=12 FUERA (se usa manual)]
[MSG:THC AUTO SIN DATO REF=110.0 MANUAL (no se estabilizo)]
[MSG:THC AUTO SIN DATO REF=110.0 MANUAL (pocas muestras)]
[MSG:THC AUTO SIN DATO REF=110.0 MANUAL (figura corta)]
```
Todo corre en primer plano (`exec_state_machine`), nunca en una ISR.
