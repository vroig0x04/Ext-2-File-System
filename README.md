# Sistema de Ficheros Ext-2

## 📋 Descripción del Proyecto

Este repositorio contiene la implementación completa de un **simulador de sistema de ficheros** basado en la estructura **ext-2 de Linux**. El proyecto constituye una implementación educativa y funcional que replica las características principales del sistema de ficheros ext-2, permitiendo operaciones de lectura, escritura, gestión de directorios y manejo de permisos en una imagen de disco virtual.

El sistema implementa:

- **Gestión de bloques de datos**: Asignación y liberación eficiente de bloques
- **Tabla de i-nodos**: Estructura de metadatos para archivos y directorios
- **Sistema de directorios**: Navegación jerárquica y gestión de entradas
- **Control de permisos**: Sistema POSIX de permisos de usuario, grupo y otros
- **Operaciones de ficheros**: Lectura, escritura, truncado y eliminación
- **Sincronización**: Uso de semáforos y mutex para operaciones thread-safe

---

**Available in other languages:** [English](README.en.md)

## 🏗️ Composición del Proyecto

```
Lenguaje C:      97.5%  | Lógica principal del simulador
Shell:            1.6%  | Scripts de compilación y ejecución
Makefile:         0.9%  | Automatización de compilación
```

## 🛠️ Herramientas y Dependencias

### Requisitos del Sistema

El simulador requiere las siguientes herramientas:

| Herramienta | Propósito | Versión Mínima |
|------------|----------|---------------|
| **GCC** | Compilador C | 7.0+ |
| **Make** | Automatizador de compilación | 4.0+ |
| **Pthread** | Librería de hilos POSIX | incluida en libc |
| **Bash** | Intérprete de scripts | 4.0+ |

### Instalación de Dependencias

#### Ubuntu/Debian:
```bash
sudo apt-get install build-essential
```

#### macOS (Homebrew):
```bash
brew install gcc make
```

#### Fedora/RHEL:
```bash
sudo dnf install gcc make glibc-devel
```

## 📁 Estructura del Repositorio

```
Ext-2-File-System/
├── Gestión de Bloques
│   ├── bloques.h                    # Definiciones de bloques
│   └── bloques.c                    # Implementación de gestión de bloques
│
├── Gestión de Ficheros
│   ├── ficheros_basico.h            # Definiciones básicas de ficheros
│   ├── ficheros_basico.c            # Operaciones básicas de ficheros
│   ├── ficheros.h                   # Definiciones avanzadas
│   └── ficheros.c                   # Operaciones avanzadas
│
├── Gestión de Directorios
│   ├── directorios.h                # Definiciones de directorios
│   └── directorios.c                # Operaciones con directorios
│
├── Operaciones I/O
│   ├── leer.c                       # Lectura de ficheros
│   ├── escribir.c                   # Escritura de ficheros
│   ├── leer_sf.c                    # Lectura de superbloque
│   └── truncar.c                    # Truncado de ficheros
│
├── Sincronización
│   ├── semaforo_mutex_posix.h       # Definiciones de sincronización
│   └── semaforo_mutex_posix.c       # Implementación de sincronización
│
├── Utilidades del Sistema
│   ├── mi_mkfs.c                    # Crear sistema de ficheros
│   ├── mi_mkdir.c                   # Crear directorio
│   ├── mi_ls.c                      # Listar contenido
│   ├── mi_stat.c                    # Información de fichero
│   ├── mi_chmod.c                   # Cambiar permisos
│   ├── mi_link.c                    # Crear enlaces
│   ├── mi_rmdir.c                   # Eliminar directorio
│   ├── mi_rm_r.c                    # Eliminar recursivamente
│   ├── mi_cat.c                     # Mostrar contenido
│   └── mi_escribir.c                # Escribir en fichero
│
├── Herramientas de Prueba
│   ├── simulacion.h/c               # Simulación del sistema
│   ├── verificacion.h/c             # Verificación de integridad
│   ├── permitir.c                   # Gestión de permisos
│   └── scripte3.sh                  # Script de prueba
│
├── Makefile                         # Automatización de compilación
├── README.md                        # Este archivo
└── texto2.txt                       # Datos de prueba
```

## 🚀 Guía de Uso

### 1. Preparación Inicial

Clone el repositorio:

```bash
git clone https://github.com/vroig0x04/Ext-2-File-System.git
cd Ext-2-File-System
```

### 2. Compilación del Sistema

Para compilar todos los programas y librerías:

```bash
make
```

Para compilar un programa específico (ejemplo):

```bash
make mi_mkfs
```

### 3. Crear un Sistema de Ficheros

Primero, cree una nueva imagen de disco ext-2:

```bash
./mi_mkfs <nombre_disco> <tamaño_en_bloques>
```

**Ejemplo:**

```bash
./mi_mkfs disco.img 1000
```

Esto crea una imagen de disco virtual de 1000 bloques.

### 4. Operaciones Básicas

#### Listar contenido de un directorio:
```bash
./mi_ls <disco> [ruta]
```

#### Crear un directorio:
```bash
./mi_mkdir <disco> <ruta_directorio>
```

#### Obtener información de un fichero:
```bash
./mi_stat <disco> <ruta_fichero>
```

#### Ver contenido de un fichero:
```bash
./mi_cat <disco> <ruta_fichero>
```

#### Cambiar permisos:
```bash
./mi_chmod <disco> <ruta> <permisos>
```

#### Crear un enlace:
```bash
./mi_link <disco> <origen> <destino>
```

#### Eliminar un directorio:
```bash
./mi_rmdir <disco> <ruta_directorio>
```

#### Eliminar recursivamente:
```bash
./mi_rm_r <disco> <ruta>
```

### 5. Limpiar la Compilación

Para eliminar los archivos compilados:

```bash
make clean
```

## 🔧 Estructura del Sistema Ext-2

### Superbloque
Contiene información global del sistema de ficheros:
- Número total de bloques y i-nodos
- Tamaño de bloque
- Estado del sistema de ficheros

### Tabla de i-nodos
Cada i-nodo contiene:
- Tipo de fichero (archivo/directorio)
- Permisos (usuario, grupo, otros)
- Dueño y grupo
- Tamaño del fichero
- Timestamps (acceso, modificación, cambio)
- Punteros a bloques de datos
- Contador de enlaces duros

### Bitmap de bloques
Rastreo de bloques libres y ocupados

### Bitmap de i-nodos
Rastreo de i-nodos libres y ocupados

### Bloques de datos
Almacenamiento real de contenido de ficheros

## 🧪 Casos de Prueba

El repositorio incluye scripts de prueba:

```bash
./scripte3.sh
```

Este script valida:
- ✅ Creación de sistemas de ficheros
- ✅ Operaciones CRUD en ficheros
- ✅ Gestión de directorios
- ✅ Control de permisos
- ✅ Integridad del sistema

## 📊 Características Principales

### Soporte de Permisos
- **Usuario (Owner)**: rwx (4, 2, 1)
- **Grupo (Group)**: rwx
- **Otros (Others)**: rwx
- Notación octal: 755, 644, 700, etc.

### Tipos de Ficheros
- Ficheros regulares
- Directorios
- Enlaces (hard links)

### Operaciones Soportadas
- Lectura/escritura de ficheros
- Truncado de contenido
- Creación/eliminación de directorios
- Navegación jerárquica
- Gestión de permisos
- Verificación de integridad

## 🔒 Sincronización y Concurrencia

El sistema utiliza **mutex POSIX** (pthread) para garantizar operaciones thread-safe:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

Esto permite múltiples procesos accediendo al sistema de ficheros de forma segura.

## ⚠️ Licencia y Derechos de Autor

Este software es propiedad intelectual de **Vicent Roig**. La copia, modificación o distribución no autorizada está estrictamente prohibida.

## 🤝 Contribuciones

Las contribuciones no son aceptadas en este momento dado el carácter propietario del proyecto.

## 📞 Contacto

Para consultas relacionadas con este proyecto, contacte al propietario del repositorio.

---

**Última actualización:** Septiembre 2026  
**Versión del Sistema:** 1.0  
**Basado en:** Ext-2 File System (Linux)
