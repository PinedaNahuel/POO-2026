========================================================
EJERCICIO 09 - COORDENADAS EN BASE DE DATOS (Qt + SQLite)
========================================================

Autor: Pineda Nahuel
Materia: POO
Tecnologias: Qt Widgets (C++), Qt Designer, SQLite

--------------------------------------------------------
1) Objetivo del proyecto
--------------------------------------------------------

Aplicacion de dibujo a mano alzada con:
- Login validado contra base de datos SQLite.
- Registro de usuarios.
- Registro de eventos de acceso (logs).
- Persistencia de trazos (coordenadas) en base de datos.

La app permite dibujar sobre un lienzo, cambiar color y grosor,
deshacer acciones y reconstruir el dibujo al reiniciar.

--------------------------------------------------------
2) Funcionalidades implementadas
--------------------------------------------------------

- Dibujo a mano alzada con mouse (clase Pintura).
- Ajuste de grosor con rueda del mouse.
- Cambio de color con teclado:
  R = rojo, G = verde, B = azul.
- Borrado total del lienzo con Escape.
- Deshacer con Ctrl+Z (hasta 10 acciones).
- Login con interfaz creada en Qt Designer (login.ui).
- Registro de nuevos usuarios desde login.
- Contraseñas guardadas con hash SHA-256.
- Logs de acceso exitoso y fallido (sin guardar contrasenias).
- Persistencia de trazos y puntos en SQLite.

--------------------------------------------------------
3) Estructura general del codigo
--------------------------------------------------------

- main.cpp
  Punto de entrada. Crea QApplication, conecta DB,
  inicializa tablas, abre Login y luego MainWindow.

- admindb.h / admindb.cpp
  Capa de acceso a datos. Maneja:
  - conexion SQLite
  - creacion de tablas
  - validacion de usuario
  - registro de usuario
  - logs
  - hash de contrasenias y migracion desde texto plano

- login.h / login.cpp / login.ui
  Ventana de inicio de sesion (Qt Designer) y logica
  de ingreso/registro.

- mainwindow.h / mainwindow.cpp
  Ventana principal que contiene el widget de dibujo
  y muestra estado en la barra inferior.

- pintura.h / pintura.cpp
  Widget de dibujo. Maneja eventos de mouse/teclado,
  deshacer, pintado y persistencia de coordenadas.

--------------------------------------------------------
4) Base de datos
--------------------------------------------------------

Motor: SQLite
Archivo: dibujo.db (junto al ejecutable)

Tablas:
- usuarios(id, usuario, clave, nombre, apellido, mail)
- logs(id, evento, fecha)
- trazos(id)
- puntos(id, trazo_id, x, y, r, g, b, grosor)

--------------------------------------------------------
5) Estado final
--------------------------------------------------------

Proyecto completo y funcional para la consigna:
- Qt Widgets
- Login con Qt Designer
- SQLite
- Persistencia de coordenadas
- Logs y seguridad basica con hash

========================================================
