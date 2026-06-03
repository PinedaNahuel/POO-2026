# Ejercicio 08 - Editor Multilenguaje (Qt Widgets)

**Aplicación de escritorio completa en C++/Qt que implementa un editor de código multilenguaje con validación sintáctica en tiempo real, login seguro y arquitectura orientada a objetos.**

Desarrollado para cumplir los requisitos de **Clase 13 - Programación Orientada a Objetos** del Curso POO 2026.

## Características principales

✅ **Sistema de autenticación**
- Login inicial con usuario `admin` / clave `1234`
├── ejercicio 8 editor multilenguaje.pro    # Configuración qmake
- Restablecimiento automático de intentos tras espera

✅ **Arquitectura polimórfica**
- Clase base abstracta `Pantalla` con interfaz común
- Clases derivadas concretas: `LoginScreen`, `ModoBloqueado`, `EditorPrincipal`
- Flujo de aplicación contra punteros a clase base (polimorfismo en tiempo de ejecución)
- Funciones virtuales puras: `inicializarUI()`, `conectarEventos()`, `cargarDatos()`, `validarEstado()`, `registrarEvento()`

✅ **Editor multilenguaje con validación**
- Selector de lenguaje: C++, Python, Java
- Validación sintáctica por línea al abandonar (Escape, Tab, Enter, focus out)
- Resaltado de errores en rojo con mensajes diagnósticos amigables
- Templates de código automáticos al cambiar lenguaje
- Validadores polimórficos: jerarquía `ValidadorSintaxis` + derivadas

✅ **Captura y redefinición de eventos**
- `keyPressEvent`: atajos de teclado (Ctrl+S exportar, ESC cerrar editor)
- `mousePressEvent`: interacción con cursor
- `resizeEvent`: adaptación visual del contenido
- `closeEvent`: confirmación de salida
- `focusInEvent` / `focusOutEvent`: control de edición y validación
- Cada pantalla implementa comportamiento distinto según su responsabilidad

✅ **Persistencia y registro**
- Lectura de configuración desde `config.ini` (usuario, tiempo de bloqueo, lenguaje por defecto, rutas)
- Auto-creación de directorios `logs/` y `salidas/` en startup
- Registro de eventos en archivo con fecha y hora: `logs/eventos.log`
- Exportación de código a un único archivo JPG legible: `salidas/codigo.jpg`

✅ **Interfaz de usuario profesional**
- Panel principal en fullscreen tras login exitoso
- Panel lateral estilo LinkedIn: foto (avatar círculo), descripción, habilidades, contacto
- Toolbar con selector de lenguaje, botones de limpiar y exportar
2. Seleccionar: ejercicio 8 editor multilenguaje.pro
- Respuesta visual inmediata a acciones del usuario

✅ **Soporte offline**
- Sin dependencias de internet o servicios remotos
- Funciona completamente local con archivos de configuración

├── config.ini              # Configuración (auto-creado si no existe)
- Usado en flujo de pantallas, eventos de editor, temporizadores, acciones de UI
- Desacoplamiento de componentes mediante señales/slots de Qt

## Requisitos de compilación

- **Qt 6.x** (probado con 6.11)
- **Compilador**: MinGW 64-bit o MSVC
## Estructura del proyecto

```
ejercicio 8 editor multilenguaje/
├── ejercicio 8 editor multilenguaje.pro    # Configuración qmake
```
    $(ZIP) ejercicio 8 editor multilenguaje.zip $(SOURCES) $(DIST) "..\..\ejercicio 8 editor multilenguaje.pro" C:\Qt\6.11.1\mingw_64\mkspecs\features\spec_pre.prf C:\Qt\6.11.1\mingw_64\mkspecs\features\device_config.prf C:\Qt\6.11.1\mingw_64\mkspecs\common\sanitize.conf C:\Qt\6.11.1\mingw_64\mkspecs\common\gcc-base.conf C:\Qt\6.11.1\mingw_64\mkspecs\common\g++-base.conf C:\Qt\6.11.1\mingw_64\mkspecs\features\win32\windows_vulkan_sdk.prf C:\Qt\6.11.1\mingw_64\mkspecs\common\windows-vulkan.conf C:\Qt\6.11.1\mingw_64\mkspecs\common\g++-win32.conf C:\Qt\6.11.1\mingw_64\mkspecs\common\windows-desktop.conf C:\Qt\6.11.1\mingw_64\mkspecs\qconfig.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_ext_freetype.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_ext_libjpeg.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_ext_libpng.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_ext_openxr_loader.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_lib_charts.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_lib_charts_private.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_lib_chartsqml.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_lib_chartsqml_private.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_lib_concurrent.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_lib_core.pri C:\Qt\6.11.1\mingw_64\mkspecs\modules\qt_lib_core_private.pri
├── Pantalla base (interfaz común)
│  └── pantalla.h / pantalla.cpp
│
├── Pantallas derivadas
│  ├── loginscreen.h / loginscreen.cpp     # Autenticación y bloqueo
│  ├── modobloqueado.h / modobloqueado.cpp # Cuenta regresiva
│  └── editorprincipal.h / editorprincipal.cpp # Editor + panel CV
│
├── Validadores polimórficos
│  ├── validadores.h / validadores.cpp    # ValidadorSintaxis + derivadas
│
├── Editor customizado
│  ├── codeeditor.h / codeeditor.cpp      # QPlainTextEdit + eventos
│
├── Orquestación
│  ├── appcontroller.h / appcontroller.cpp # Flujo de pantallas
│  ├── appconfig.h / appconfig.cpp         # Lectura/creación config.ini
│  └── eventlogger.h / eventlogger.cpp     # Registro de eventos
│
├── Configuración
│  └── config.ini                          # Auto-generado si no existe
│
└── Documentación
    ├── README.md                          # Este archivo
    └── IMPLEMENTACION.txt                 # Detalles técnicos
```

## Compilación en Qt Creator

### Paso 1: Abrir proyecto
```bash
1. Qt Creator → Archivo → Abrir archivo o proyecto
2. Seleccionar: ejercicio 8 editor multilenguaje.pro
```

### Paso 2: Configurar kit
```bash
1. En el panel izquierdo, seleccionar kit "Desktop Qt 6.x MinGW 64-bit" (o tu kit disponible)
2. Si no aparece, crear uno: Herramientas → Opciones → Kits → Agregar
```

### Paso 3: Compilar y ejecutar
```bash
1. Build: Ctrl + B (o Build → Build Project)
2. Run: Ctrl + R (o Build → Run)
```

## Uso de la aplicación

### 1. Pantalla de Login
- Ingresar usuario: `admin`
- Ingresar contraseña: `1234`
- Presionar "Ingresar" o Enter

**Si falla** (3 intentos máximo):
- Se activa modo bloqueado con cuenta regresiva de 15 segundos (configurable)
- Se permite reintentar tras finalizar la espera

### 2. Editor Principal
- **Selector de lenguaje**: dropdown C++ / Python / Java
- **Editor**: escribir código en el lenguaje seleccionado
- **Validación**: se ejecuta automáticamente al abandonar línea
  - Línea válida: sin marca
  - Línea inválida: resaltado rojo + mensaje diagnóstico

### 3. Atajos de teclado
| Atajo | Acción |
|-------|--------|
| `Enter` | Intentar login (en LoginScreen) |
| `Ctrl+S` | Exportar código a JPG |
| `ESC` | Cerrar editor principal |

### 4. Exportación
- Botón "Exportar JPG (Ctrl+S)"
- Guarda a: `salidas/codigo.jpg`
- Se muestra ruta final en diálogo de confirmación

## Archivos generados en ejecución

```
ejercicio 8 editor multilenguaje/
├── config.ini              # Configuración (auto-creado si no existe)
├── logs/
│  └── eventos.log          # Registro de eventos con timestamp
└── salidas/
   └── codigo.jpg           # Exportación del código escrito
```

## Configuración (config.ini)

```ini
[auth]
usuario=admin
clave=1234
tiempoBloqueoSegundos=15

[editor]
lenguajePorDefecto=C++
rutaExportacion=salidas/codigo.jpg

[app]
rutaLog=logs/eventos.log
```

**Modificar valores** para cambiar credenciales, tiempo de bloqueo o rutas de exportación/log.

## Validación de sintaxis por lenguaje

### C++
✓ Comentarios `//`
✓ Palabras clave: `if`, `for`, `while`, `class`, `namespace`, `void`, `int`, etc.
✓ Terminación con `;` o bloques `{ }`
✗ Rechaza: `print()`, `def`, `class x:`, `System.out`, palabras de Python/Java

### Python
✓ Comentarios `#`
✓ Palabras clave: `if`, `for`, `while`, `def`, `class`, `try`, etc.
✓ Indentación en múltiplos de 4
✓ Bloques terminados con `:`
✗ Rechaza: `cout`, `void`, `System.out`, `{}`; palabras de C++/Java

### Java
✓ Comentarios `//`
✓ Palabras clave: `public`, `class`, `if`, `for`, `while`, etc.
✓ Terminación con `;` o bloques `{ }`
✗ Rechaza: `print()`, `def`, `:` al final; palabras de Python/C++

**Nota**: La validación es **heurística y didáctica** por línea. No reemplaza compiladores reales.

## Ejemplo de uso completo

```
1. Iniciar app → Pantalla de Login
2. Usuario: admin
3. Contraseña: 1234
4. Enter → Pantalla principal en fullscreen
5. Selector de lenguaje: C++
6. Template inicial:
   // Escribi tu codigo aqui
   #include <iostream>
   int main() {
       std::cout << "Hola mundo" << std::endl;
       return 0;
   }
7. Editar código → líneas se validan automáticamente
8. Escribir linea incorrecta (ej: cout sin std::) → rojo + diagnostico
9. Corregir → color normal
10. Ctrl+S → Exportar a JPG
11. ESC → Cerrar editor
12. Volver a login (permite reintentar si no se bloqueó)
```

## Notas de desarrollo

- **Polimorfismo**: Clase `Pantalla` es base abstracta usada en `AppController` para manejar flujo. Cada derivada implementa interfaz común.
- **Validadores**: Jerarquía `ValidadorSintaxis` → `ValidadorCpp/Python/Java` demuestra polimorfismo adicional.
- **Events**: Cada pantalla redefine eventos según su rol (login no responde ESC, editor sí).
- **Log**: Registro persistente con formato `[YYYY-MM-DD HH:MM:SS] | descripción`
- **UI**: Responsive a cambios (resize, focus, validación) sin bloqueos.

## Limitaciones y consideraciones

- Validación por línea es **simplificada**: no analiza contexto multi-línea (bucles incompletos, etc.)
- Exportación JPG respeta saltos de línea pero no coloreado sintáctico (blanco y negro)
- Sistema offline: no sincroniza datos a la nube
- Credenciales quemadas en config.ini (en producción usar hashing y BD)

## Troubleshooting

| Problema | Solución |
|----------|----------|
| No compila: "header not found" | Verificar que Qt esté correctamente instalado y kit seleccionado |
| `config.ini` no se crea | Verificar permisos de escritura en directorio de ejecución |
| Log vacío | Verificar permisos de `logs/` o que exista directorio |
| JPG no se guarda | Verificar permisos en `salidas/` y espacio disco |
| Validación muy estricta | Es por diseño; editar `validadores.cpp` si se desea cambiar reglas |

## Conclusión

Esta aplicación demuestra:
✅ Uso real de **herencia y polimorfismo** (2 jerarquías)
✅ **Arquitectura escalable** (pantallas y validadores independientes)
✅ **Manejo de eventos** de Qt (6 tipos redefinidos)
✅ **Signals/Slots** para desacoplamiento
✅ **Persistencia local** (config + log)
✅ **UI responsiva** con feedback en tiempo real
✅ **Exportación de datos** en formato estándar (JPG)

Pronta para **evaluación y publicación** en portafolio profesional.

---

*Desarrollado con Qt 6.x / qmake / C++17*  
*Curso POO 2026 - Clase 13*  
*Agosto 2026*

