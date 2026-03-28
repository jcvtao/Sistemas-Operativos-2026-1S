# Sistemas Operativos Parcial 1

## Integrantes

- Yadixon David Alfonso Chiquillo ([yalfonsoc@unal.edu.co](mailto:yalfonsoc@unal.edu.co))
- Federico Puentes Acosta ([fpuentesa@unal.edu.co](mailto:fpuentesa@unal.edu.co))
- Juan Camilo Vergara Tao ([juvergarat@unal.edu.co](mailto:juvergarat@unal.edu.co))


## Enunciado

- Google Drive (UNAL): [Parcial 1](https://drive.google.com/file/d/1la6NzH3UoP6NxSa3R0Dq9Elo5zXw_9h2/view?usp=sharing)
- GitHub: [Parcial 1](https://github.com/jcvtao/Sistemas-Operativos-2026-1S/blob/main/Parcial-1/parcial1.pdf)


## Documentación

Este programa vigila un archivo específico, cuyo nombre se pasa desde la consola como argumento, y realiza una copia de seguridad automática en la carpeta `./data/backup/` cada vez que se detecta una modificación en su contenido.

<!---
### Diagrama de Bloques
TODO
--->

### Compilación

Ejecuta el comando:
```bash
make
```

> [!TIP]
> Si desea eliminar el ejecutable y la copia de seguridad puede ejecutar el comando `make clean`.

### Ejecución

Ejecuta el comando:
```bash
./bin/backup <archivo_a_vigilar>
```

En el caso del presente repositorio el archivo de prueba es `archivo-prueba.txt`, por lo cuál se debe ejecutar:
```bash
./bin/backup archivo-prueba.txt
```

Cuando el contenido de `archivo-prueba.txt` cambie se creará una copia de seguridad en `data/backup/archivo-prueba.txt.bak`.