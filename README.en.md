# Ext-2 File System

## 📋 Project Description

This repository contains the complete implementation of a **file system simulator** based on the **ext-2 structure of Linux**. The project constitutes an educational and fully functional implementation that replicates the main characteristics of the ext-2 file system, allowing read, write, directory management, and permission handling operations on a virtual disk image.

The system implements:

- **Block management**: Efficient allocation and deallocation of blocks
- **i-node table**: Metadata structure for files and directories
- **Directory system**: Hierarchical navigation and entry management
- **Permission control**: POSIX permissions system for user, group, and others
- **File operations**: Reading, writing, truncating, and deletion
- **Synchronization**: Use of semaphores and mutexes for thread-safe operations

---

**Disponible en otros idiomas:** [Español](README.md)

##  Project Composition

```
C Language:      97.5%  | Main simulator logic
Shell:            1.6%  | Compilation and execution scripts
Makefile:         0.9%  | Build automation
```

##  Tools and Dependencies

### System Requirements

The simulator requires the following tools:

| Tool | Purpose | Minimum Version |
|------|---------|------------------|
| **GCC** | C Compiler | 7.0+ |
| **Make** | Build automation | 4.0+ |
| **Pthread** | POSIX threads library | included in libc |
| **Bash** | Shell interpreter | 4.0+ |

### Installing Dependencies

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

## 📁 Repository Structure

```
Ext-2-File-System/
├── Block Management
│   ├── bloques.h                    # Block definitions
│   └── bloques.c                    # Block management implementation
│
├── File Management
│   ├── ficheros_basico.h            # Basic file definitions
│   ├── ficheros_basico.c            # Basic file operations
│   ├── ficheros.h                   # Advanced definitions
│   └── ficheros.c                   # Advanced operations
│
├── Directory Management
│   ├── directorios.h                # Directory definitions
│   └── directorios.c                # Directory operations
│
├── I/O Operations
│   ├── leer.c                       # File reading
│   ├── escribir.c                   # File writing
│   ├── leer_sf.c                    # Superblock reading
│   └── truncar.c                    # File truncation
│
├── Synchronization
│   ├── semaforo_mutex_posix.h       # Synchronization definitions
│   └── semaforo_mutex_posix.c       # Synchronization implementation
│
├── System Utilities
│   ├── mi_mkfs.c                    # Create file system
│   ├── mi_mkdir.c                   # Create directory
│   ├── mi_ls.c                      # List contents
│   ├── mi_stat.c                    # File information
│   ├── mi_chmod.c                   # Change permissions
│   ├── mi_link.c                    # Create links
│   ├── mi_rmdir.c                   # Remove directory
│   ├── mi_rm_r.c                    # Remove recursively
│   ├── mi_cat.c                     # Display contents
│   └── mi_escribir.c                # Write to file
│
├── Testing Tools
│   ├── simulacion.h/c               # System simulation
│   ├── verificacion.h/c             # Integrity verification
│   ├── permitir.c                   # Permission management
│   └── scripte3.sh                  # Test script
│
├── Makefile                         # Build automation
├── README.en.md                     # This file
└── texto2.txt                       # Test data
```

##  Usage Guide

### 1. Initial Setup

Clone the repository:

```bash
git clone https://github.com/vroig0x04/Ext-2-File-System.git
cd Ext-2-File-System
```

### 2. System Compilation

To compile all programs and libraries:

```bash
make
```

To compile a specific program (example):

```bash
make mi_mkfs
```

### 3. Create a File System

First, create a new ext-2 disk image:

```bash
./mi_mkfs <disk_name> <size_in_blocks>
```

**Example:**

```bash
./mi_mkfs disco.img 1000
```

This creates a virtual disk image of 1000 blocks.

### 4. Basic Operations

#### List directory contents:
```bash
./mi_ls <disk> [path]
```

#### Create a directory:
```bash
./mi_mkdir <disk> <directory_path>
```

#### Get file information:
```bash
./mi_stat <disk> <file_path>
```

#### View file contents:
```bash
./mi_cat <disk> <file_path>
```

#### Change permissions:
```bash
./mi_chmod <disk> <path> <permissions>
```

#### Create a link:
```bash
./mi_link <disk> <source> <destination>
```

#### Remove a directory:
```bash
./mi_rmdir <disk> <directory_path>
```

#### Remove recursively:
```bash
./mi_rm_r <disk> <path>
```

### 5. Clean Compilation

To remove compiled files:

```bash
make clean
```

## 🔧 Ext-2 File System Structure

### Superblock
Contains global file system information:
- Total number of blocks and i-nodes
- Block size
- File system status

### i-node Table
Each i-node contains:
- File type (file/directory)
- Permissions (user, group, others)
- Owner and group
- File size
- Timestamps (access, modification, change)
- Pointers to data blocks
- Hard link counter

### Block Bitmap
Tracking of free and occupied blocks

### i-node Bitmap
Tracking of free and occupied i-nodes

### Data Blocks
Actual storage of file contents

##  Test Cases

The repository includes test scripts:

```bash
./scripte3.sh
```

This script validates:
-  File system creation
-  CRUD operations on files
-  Directory management
-  Permission control
-  System integrity

##  Main Features

### Permission Support
- **User (Owner)**: rwx (4, 2, 1)
- **Group (Group)**: rwx
- **Others (Others)**: rwx
- Octal notation: 755, 644, 700, etc.

### File Types
- Regular files
- Directories
- Links (hard links)

### Supported Operations
- File read/write
- Content truncation
- Directory creation/deletion
- Hierarchical navigation
- Permission management
- Integrity verification

##  Synchronization and Concurrency

The system uses **POSIX mutexes** (pthread) to ensure thread-safe operations:

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```

This allows multiple processes to access the file system safely.

## ⚠️ License and Copyright

This software is the intellectual property of **Vicent Roig**. Unauthorized copying, modification, or distribution is strictly prohibited.

## 🤝 Contributing

Contributions are not accepted at this time given the proprietary nature of the project.

## 📞 Contact

For inquiries related to this project, please contact the repository owner.

---

**Last Updated:** September 2026  
**System Version:** 1.0  
**Based on:** Ext-2 File System (Linux)
