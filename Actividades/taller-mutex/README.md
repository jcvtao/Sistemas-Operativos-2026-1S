# Taller Mutex

## Enunciado

- Google Drive (UNAL): [Guía Mutex](https://drive.google.com/file/d/1HiiYosKmnhEnRXftla2vI7a6fX5IhRqK/view?usp=sharing)
- GitHub: [Guía Mutex](https://github.com/jcvtao/Sistemas-Operativos-2026-1S/blob/main/Actividades/taller-mutex/guia_mutex.pdf)


## Análisis

### Código sin Mutex

Al ejecutar el código con 8 hilos, se observa un comportamiento **no determinista**. Los números impresos por los diferentes hilos se mezclan (e.g. `001112222...`). Esto ocurre porque todos los hilos intentan escribir en el recurso compartido `stdout` al mismo tiempo. Debido a que no hay un Mutex, un hilo puede ser interrumpido por el planificador del SO justo después de imprimir sus números, permitiendo que otro hilo imprima los suyos antes de que el primero retome para imprimir sus guiones.

Con pocos hilos el entrelazado es menos evidente pero sigue existiendo, es más fácil seguir el rastro de la ejecución. Con muchos hilos el caos aumenta drásticamente, el tiempo de respuesta parece mayor y la consola muestra ráfagas de números y guiones totalmente desordenadas.


### Código con Mutex

Se implementó un mecanismo de exclusión mutua (Mutex) para proteger la sección de impresión. A diferencia de la ejecución concurrente desordenada, el uso de `pthread_mutex_lock` y `pthread_mutex_unlock` garantiza que solo un hilo tenga acceso al recurso `stdout` a la vez. Cada hilo completa su bloque de instrucciones de forma ininterrumpida.

Aunque los hilos se ejecutan en paralelo, la sección crítica se vuelve secuencial. Esto elimina las condiciones de carrera, pero incrementa el tiempo total de ejecución del programa, ya que los hilos deben "hacer fila" para entrar a la sección protegida. El orden de salida (e.g. `00 - - 111 - - - 222...`) tiende a ser más estable, pero no siempre porque el planificador del sistema operativo sigue siendo el que decide qué hilo de la cola toma el mutex primero una vez que queda libre. Con menos hilos se nota menos estabilidad y con más hilos parece ser más estable el orden de salida.


## Instrucciones de Ejecución

1. Compilar: `make`

2. Ejecutar código sin mutex: `./taller-mutex-1`

3. Ejecutar código con mutex: `./taller-mutex-2`

>[!TIP]
> Si desea eliminar los archivos ejecutables puede hacer `make clean`