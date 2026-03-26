# Planificador de Trabajos Prácticos (Qt)

Aplicación de escritorio desarrollada en C++ con Qt Widgets para gestionar trabajos prácticos, incluyendo autenticación de usuarios, seguimiento de tareas y persistencia local en archivos JSON.

## Funcionalidades

- Login con usuarios almacenados en JSON
- Persistencia de datos local (`users.json`, `tasks.json`, `history.json`)
- Lectura y escritura de datos mediante `QJson`
- Estructura modular (models y services)

## Estructura del proyecto

- `models/` → Clases de datos (`User`, `Task`, `HistoryEntry`)
- `services/` → Manejo de archivos (`JsonStorage`)
- `data/` → Archivos JSON (base de datos local)

## Ejecución

⚠️ Importante:  
La carpeta `data/` debe estar ubicada en el mismo directorio donde se ejecuta el `.exe` (carpeta `build`).

## Tecnologías

- C++
- Qt Widgets
- JSON (QJsonObject, QJsonDocument)

## Autor

Proyecto académico - Ingeniería