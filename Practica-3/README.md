# Práctica 3

## Integrantes

- Yadixon David Alfonso Chiquillo ([yalfonsoc@unal.edu.co](mailto:yalfonsoc@unal.edu.co))
- Federico Puentes Acosta ([fpuentesa@unal.edu.co](mailto:fpuentesa@unal.edu.co))
- Juan Camilo Vergara Tao ([juvergarat@unal.edu.co](mailto:juvergarat@unal.edu.co))


## Enunciado

El enunciado de la práctica se encuentra en los siguientes enlaces:
- [Práctica 3 (Repositorio de César Pedraza)](https://capedrazab.github.io/material_cursos/20261/os/practica3.pdf)
- [Práctica 3 (Repositorio del equipo)](https://github.com/jcvtao/Sistemas-Operativos-2026-1S/blob/main/Practica-3/practica3.pdf)


## Dataset

El dataset utilizado es el mismo de la [práctica anterior](https://github.com/jcvtao/Sistemas-Operativos-2026-1S/tree/main/Practica-1#dataset): información de más de **7 millones de empresas** vinculadas a LinkedIn a nivel global. El archivo original en formato CSV (`companies-sorted.csv`) tiene un tamaño aproximado de **1.09 GB**.

Enlace de descarga: [Kaggle: 7+ Million Company Dataset](https://www.kaggle.com/datasets/peopledatalabssf/free-7-million-company-dataset?resource=download)


### Campos del Dataset

| # | Campo | Tipo | Descripción |
|---|-------|------|-------------|
| 1 | # | int | Identificador de la empresa en el dataset. |
| 2 | name | string | Nombre de la empresa. |
| 3 | domain | string | Dominio web de la empresa. |
| 4 | year founded | int | Año de creación. |
| 5 | industry | string | Industria o sector económico al que pertenece la empresa. |
| 6 | size range | string | Rango de empleados definido por la cantidad total de empleados. |
| 7 | locality | string | Ubicación de la empresa incluyendo ciudad, estado y país. |
| 8 | country | string | País donde se ubica la empresa. |
| 9 | linkedIn url | string | URL de LinkedIn asociado con la empresa. |
| 10 | current employee estimate | int | Número de empleados actuales de la empresa. |
| 11 | total employee estimate | int | Número total de empleados de la empresa. |


## Implementación Técnica

### Arquitectura cliente-servidor

El sistema se compone de dos programas independientes que se comunican a través de **sockets TCP/IP**:

- **`p2-server`**: gestiona los archivos de índice y `data.bin`. Escucha conexiones entrantes en el puerto 8080 y puede atender hasta 32 clientes simultáneos. Por cada cliente que se conecta, crea un proceso hijo con `fork()` para atender sus peticiones sin bloquear al resto; los procesos hijo terminados se recogen mediante el manejador de `SIGCHLD` con `waitpid()`. Adicionalmente, registra cada búsqueda en un archivo log (`search_history.log`) con la IP del cliente y la fecha.

- **`p2-client`**: muestra el menú de búsqueda al usuario, se conecta al servidor al iniciar y mantiene la conexión abierta durante toda la sesión. Al salir con la opción `[3]`, cierra el socket.

### Criterios de Búsqueda

Se mantienen los dos criterios de búsqueda de la [práctica anterior](https://github.com/jcvtao/Sistemas-Operativos-2026-1S/tree/main/Practica-1#criterios-de-b%C3%BAsqueda):

1. **Búsqueda por nombre:** tabla hash de 500,000 entradas indexada sobre el campo `name`.

2. **Búsqueda por país e industria:** tabla hash de 500,000 entradas indexada sobre la llave compuesta `país + industria`.

### Gestión de Memoria

Para que el proceso servidor no supere **1 MB de RAM** los archivos de índice (`index_name.bin` e `index_country_industry.bin`) no se cargan en memoria, sino que el servidor los abre con `open()` y usa `pread()` para leer únicamente el offset de la tabla hash. El resto de la cadena de colisiones se recorre sobre `data.bin` con `fseek()`, igual que en la [práctica anterior](https://github.com/jcvtao/Sistemas-Operativos-2026-1S/tree/main/Practica-1#criterios-de-b%C3%BAsqueda).

Los resultados de cada búsqueda sí se acumulan en un buffer dinámico mediante `malloc()`, `realloc()` y `free()`:

- `malloc()` reserva espacio para `INITIAL_CAP` empresas al inicio de cada búsqueda.
- `realloc()` duplica la capacidad del buffer cada vez que se llena.
- `free()` libera el buffer una vez enviados todos los resultados al cliente.

El mismo patrón se aplica en el cliente para recibir y almacenar los resultados antes de mostrarlos.

### Archivo Log

El servidor registra cada petición en `search-history.log` con el siguiente formato:

```
[Fecha YYYYMMDDTHHMMSS] Cliente [IP] [tipo_de_búsqueda - llave_1 - llave_2]
```

Ejemplo:
```
[20260513T142301] Cliente [127.0.0.1] [nombre - google]
[20260513T142315] Cliente [127.0.0.1] [pais_industria - india - retail]
```


## Instrucciones de Uso

### Requisitos
1. Tener instalado `gcc` y `make`.
2. Descargar el dataset [`companies-sorted.csv`](https://www.kaggle.com/datasets/peopledatalabssf/free-7-million-company-dataset?resource=download) y ubicarlo en `data/companies-sorted.csv`.

### Compilación
En la raíz del proyecto, ejecutar:
```bash
make
```

> [!TIP]
> Si desea eliminar correctamente los ejecutables y el log para volver a compilar puede ejecutar el comando
> ```bash
> make clean
> ```

### Ejecución

El servidor y el cliente son programas independientes que deben ejecutarse en **terminales separadas**.

#### Paso 1 - Indexar el dataset (solo la primera vez)
```bash
./bin/indexer
```
Genera `bin/data.bin`, `bin/index-name.bin` y `bin/index-country-industry.bin`.

#### Paso 2 - Iniciar el servidor
```bash
./bin/p2-server
```
El servidor queda en espera. No se cierra hasta recibir una señal de interrupción (`Ctrl+C`).

#### Paso 3 — Iniciar el cliente (en otra terminal)
```bash
./bin/p2-client
```
Se abre el menú de búsqueda y se obtienen los resultados en la terminal. Para conectarse a un servidor remoto se debe modificar la constante `SERVER_IP` en `p2-client.c` antes de compilar.

### Rangos Válidos de Búsqueda

- **Nombres, países e industrias:** cadenas alfabéticas (a-z) en minúsculas, escritas en inglés. Se permiten espacios y comas en medio, pero no al inicio.
- **Opciones de menú:** valores numéricos (1, 2 o 3 según corresponda).

### Ejemplos de Uso

- **Búsqueda por nombre:** Seleccionar opción `1`, ingresar `google`. El sistema retornará toda la información relacionada a Google presente en el dataset, que corresponde a 4 resultados.

    ![Captura de búsqueda por nombre](img/busqueda_nombre.png)

- **Búsqueda por país e industria:** Seleccionar opción `2`, escribir para el país `india` y para la industria `retail`. Retornará todas las empresas que coincidan con ambos filtros simultáneamente, obteniéndose la información de 1491 empresas.

    ![Captura de búsqueda por país e industria](img/busqueda_pais_industria.png)