# Práctica 2: Cliente-Servidor con Sockets TCP

## Integrantes

- Yadixon David Alfonso Chiquillo ([yalfonsoc@unal.edu.co](mailto:yalfonsoc@unal.edu.co))
- Federico Puentes Acosta ([fpuentesa@unal.edu.co](mailto:fpuentesa@unal.edu.co))
- Juan Camilo Vergara Tao ([juvergarat@unal.edu.co](mailto:juvergarat@unal.edu.co))


## Enunciado

El enunciado de la práctica se encuentra en los siguientes enlaces:
- [Práctica 3 (Repositorio de César Pedraza)](https://capedrazab.github.io/material_cursos/20261/os/practica3.pdf)
- [Práctica 3 (Repositorio del equipo)](https://github.com/jcvtao/Sistemas-Operativos-2026-1S/blob/main/Practica-3/practica3.pdf)


## Dataset

El dataset utilizado es el mismo de la práctica anterior: información de más de **7 millones de empresas** vinculadas a LinkedIn a nivel global. El archivo original en formato CSV (`companies_sorted.csv`) tiene un tamaño aproximado de **1.09 GB**.

Enlace de descarga: [Kaggle: 7+ Million Company Dataset](https://www.kaggle.com/datasets/peopledatalabssf/free-7-million-company-dataset?resource=download)


## Implementación Técnica

### Arquitectura cliente-servidor

El sistema se compone de dos programas independientes que se comunican a través de **sockets TCP/IP**:

- **`p2-server`**: gestiona los archivos de índice y `data.bin`. Escucha conexiones entrantes en el puerto 8080 y puede atender hasta 32 clientes simultáneos. Por cada cliente que se conecta, crea un proceso hijo con `fork()` para atender sus peticiones sin bloquear al resto. Los procesos hijo terminados se recogen mediante el manejador de `SIGCHLD` con `waitpid()`, evitando procesos zombie. Adicionalmente, registra cada búsqueda en un archivo log con la IP del cliente y la fecha.

- **`p2-client`**: muestra el menú de búsqueda al usuario, se conecta al servidor al iniciar y mantiene la conexión abierta durante toda la sesión. Al salir con la opción `[3]`, cierra el socket.

### Criterios de Búsqueda

Se mantienen los dos criterios de búsqueda de la práctica anterior:

1. **Búsqueda por nombre:** tabla hash de 500,000 entradas indexada sobre el campo `name`.

2. **Búsqueda por país e industria:** tabla hash de 500,000 entradas indexada sobre la llave compuesta `país + industria`.

### Gestión de Memoria

El enunciado exige que el proceso servidor no supere **1 MB de RAM**. Para cumplirlo, los archivos de índice (`index_name.bin` e `index_country_industry.bin`) **no se cargan en memoria**: el servidor los abre con `open()` y usa `pread()` para leer únicamente el offset del cubeto hash necesario (8 bytes por búsqueda). El resto de la cadena de colisiones se recorre sobre `data.bin` con `fseek()`, igual que en la práctica anterior.

Los resultados de cada búsqueda sí se acumulan en un buffer dinámico mediante `malloc()`, `realloc()` y `free()`:

- `malloc()` reserva espacio para `INITIAL_CAP` empresas al inicio de cada búsqueda.
- `realloc()` duplica la capacidad del buffer cada vez que se llena.
- `free()` libera el buffer una vez enviados todos los resultados al cliente.

El mismo patrón se aplica en el cliente para recibir y almacenar los resultados antes de mostrarlos.

| Elemento | Práctica anterior | Esta práctica |
|---|---|---|
| Comunicación | FIFOs (local) | Sockets TCP/IP (red) |
| Índices en RAM | `malloc` → 7.6 MB | `open` + `pread` → 0 MB |
| Resultados | Enviados uno a uno | Buffer `malloc`/`realloc` |
| Concurrencia | Un proceso fijo | `fork()` por cliente |

### Archivo Log

El servidor registra cada petición en `search_history.log` con el siguiente formato:

```
[Fecha YYYYMMDDTHHMMSS] Cliente [IP] [tipo - clave1 - clave2]
```

Ejemplo:
```
[Fecha 20260513T142301] Cliente [127.0.0.1] [nombre - google - ]
[Fecha 20260513T142315] Cliente [127.0.0.1] [pais_industria - india - retail]
```


## Instrucciones de Uso

### Requisitos
1. Tener instalado `gcc` y `make`.
2. Descargar el dataset [`companies_sorted.csv`](https://www.kaggle.com/datasets/peopledatalabssf/free-7-million-company-dataset?resource=download) y ubicarlo en `data/companies_sorted.csv`.

### Compilación
En la raíz del proyecto, ejecutar:
```bash
make
```

> [!TIP]
> Para eliminar los ejecutables y volver a compilar desde cero:
> ```bash
> make clean && make
> ```

### Ejecución

El servidor y el cliente son programas independientes que deben ejecutarse en **terminales separadas**.

#### Paso 1 — Indexar el dataset (solo la primera vez)
```bash
./bin/indexer
```
Genera `bin/data.bin`, `bin/index_name.bin` y `bin/index_country_industry.bin`. Con 7 millones de registros tarda varios minutos.

#### Paso 2 — Iniciar el servidor
```bash
./bin/p2-server
```
El servidor queda en espera. No se cierra hasta recibir una señal de interrupción (`Ctrl+C`).

#### Paso 3 — Iniciar el cliente (en otra terminal)
```bash
./bin/p2-client
```
Para conectarse a un servidor remoto, modificar la constante `SERVER_IP` en `p2-client.c` antes de compilar.

### Rangos Válidos de Búsqueda

- **Nombres, países e industrias:** cadenas alfabéticas (a-z) en minúsculas, escritas en inglés. Se permiten espacios y comas en medio, pero no al inicio.
- **Opciones de menú:** valores numéricos (1, 2 o 3 según corresponda).

### Ejemplos de Uso

- **Búsqueda por nombre:** seleccionar opción `[1]`, ingresar `google`. Retorna 4 resultados.

- **Búsqueda por país e industria:** seleccionar opción `[2]`, ingresar `india` como país y `retail` como industria. Retorna 1491 resultados.