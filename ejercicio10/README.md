# T-Rex Extremo - Ejercicio 10

Pequeño juego en Qt Widgets inspirado en el T-Rex de Chrome.

Archivos creados:
- main.cpp
- game.h / game.cpp
- trex.h / trex.cpp
- pajaro.h / pajaro.cpp
- ejercicio10.pro

Compilar (qmake):

```bash
qmake ejercicio10.pro
make
./ejercicio10
```

Controles:
- Espacio: saltar
- Flecha Abajo: agacharse
- Flecha Derecha: acelerar
- Flecha Izquierda: frenar
- R: reiniciar después de Game Over

Notas:
- Los pájaros (`Pajaro`) tienen cada uno su propio `QTimer` en `pajaro.cpp`.
- El `QTimer` principal en `GameWindow` mueve los cactus y detecta colisiones.
- No se incluyen imágenes externas; los sprites se dibujan por código.
