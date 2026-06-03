README - EJERCICIO 03: LIENZO COLABORATIVO EN TIEMPO REAL
=========================================================

AUTORES
-----
Augusto Landerreche
Drako Lusicic
Nahuel Agustin Pineda

DESCRIPCION GENERAL
-------------------
Este proyecto implementa una aplicacion de escritorio en Qt/C++ que permite
dibujar a mano alzada sobre un lienzo y sincronizar el dibujo entre varios
usuarios mediante un backend alojado en un VPS.

La aplicacion fue desarrollada para cumplir la consigna del ejercicio:
- lienzo implementado con paintEvent
- dibujo libre a mano alzada
- goma con click derecho
- grosor con rueda del mouse
- seleccion de color con teclas 1 al 9
- barra superior con boton Guardar
- recuperacion del dibujo almacenado al iniciar
- sincronizacion colaborativa con servidor
- arquitectura separada en modelo, vista y sincronizacion

FUNCIONAMIENTO DE LA APP
------------------------
Al iniciar la aplicacion, se solicita un nombre de usuario.

Ese nombre se usa para identificar los trazos creados por ese cliente. Esto
permite que cada usuario pueda borrar solamente sus propios trazos, incluso
cuando el dibujo ya fue guardado y luego recuperado desde el servidor.

La app descarga el estado actual del dibujo al iniciar. Luego, cada usuario
puede seguir dibujando localmente y guardar manualmente con el boton Guardar.

CONTROLES
---------
- Click izquierdo: dibujar
- Click derecho: borrar con goma
- Scroll del mouse: cambia el grosor de la herramienta activa
- Teclas 1 a 9: cambia el color del pincel

PALETA DE COLORES
-----------------
- 1: rojo
- 2: azul
- 3: negro
- 4: violeta
- 5: naranja
- 6: verde
- 7: amarillo
- 8: rosa
- 9: gris suave

COMPORTAMIENTO DE DIBUJO Y GOMA
-------------------------------
El dibujo se realiza como trazo continuo, usando los puntos capturados del
mouse para renderizar lineas suaves con extremos redondeados.

La goma funciona como una herramienta de borrado continuo, similar a Paint.
Internamente, el sistema conserva la restriccion por usuario: los trazos de
goma solo afectan los dibujos creados por el usuario logueado.

ARQUITECTURA DEL PROYECTO
-------------------------
El proyecto esta dividido en tres partes principales:

1. Modelo
	 Archivo: canvasmodel.h / canvasmodel.cpp
	 Responsabilidades:
	 - almacenar trazos
	 - serializar y deserializar a JSON
	 - mantener informacion de color, grosor y usuario propietario
	 - generar la imagen final del lienzo

2. Vista
	 Archivo: canvasview.h / canvasview.cpp
	 Responsabilidades:
	 - manejar paintEvent
	 - capturar mouse, rueda y teclado
	 - dibujar pincel y goma
	 - mostrar el lienzo en pantalla

3. Sincronizacion
	 Archivo: syncmanager.h / syncmanager.cpp
	 Responsabilidades:
	 - comunicarse con el backend HTTP
	 - cargar el dibujo desde el servidor
	 - enviar el dibujo al servidor al guardar
	 - polling automatico para refresco remoto

Ademas, la ventana principal se encuentra en:
- mainwindow.h / mainwindow.cpp

y el punto de entrada en:
- main.cpp

ESTRUCTURA DE ARCHIVOS PRINCIPALES
----------------------------------
- ejercicio 3 lienzo colaborativo en tiempo real.pro
- main.cpp
- mainwindow.h
- mainwindow.cpp
- canvasmodel.h
- canvasmodel.cpp
- canvasview.h
- canvasview.cpp
- syncmanager.h
- syncmanager.cpp

Tambien se incluyen archivos de apoyo:
- tutorial_servidor.txt
- tutorial_cliente.txt
- tutorial_colaboracion.txt
- tutorial_troubleshooting.txt

REQUISITOS
----------
Para el cliente:
- Qt Creator
- Qt 5.15 o superior
- modulos Qt: core, gui, widgets, network

Para el backend:
- Python 3
- Flask
- VPS Linux (Ubuntu/Debian recomendado)

COMPILACION EN QT CREATOR
-------------------------
1. Abrir el archivo ejercicio 3 lienzo colaborativo en tiempo real.pro en Qt Creator.
2. Seleccionar el kit de compilacion correspondiente.
3. Build Project.
4. Run.

Tambien se puede compilar por consola con qmake + make, aunque la entrega fue
preparada para ejecutarse desde Qt Creator.

CONFIGURACION DEL SERVIDOR
--------------------------
La aplicacion cliente se conecta a un backend HTTP que expone dos endpoints:

- GET /load
	Devuelve el estado actual del dibujo en JSON.

- POST /save
	Recibe el estado completo del dibujo y lo guarda en el servidor.

La URL base del servidor se configura en mainwindow.cpp.

Ejemplo:
		m_sync->setServerUrl(QUrl("http://TU_VPS:5000"));

El backend utilizado para las pruebas fue implementado en Python + Flask y
guarda el estado en un archivo JSON.

FORMATO DE DATOS
----------------
Cada trazo contiene:
- id unico
- lista de puntos
- color
- grosor
- tipo de herramienta (lapiz o goma)
- usuario propietario

Esto permite:
- reconstruir el dibujo al cargar
- mantener persistencia
- restringir el borrado al usuario dueño del trazo

FLUJO DE USO
------------
1. El usuario abre la app.
2. Ingresa su nombre de usuario.
3. La app descarga el dibujo actual del servidor.
4. El usuario dibuja o borra en el lienzo.
5. Al presionar Guardar, se envian los cambios al VPS.
6. Otros usuarios pueden cargar o refrescar el contenido compartido.

COLABORACION
------------
La colaboracion se resuelve mediante un modelo incremental de trazos.

Cada cliente trabaja localmente sobre su copia del lienzo y puede persistir el
estado en el servidor. El sistema mantiene informacion suficiente para:
- preservar el dibujo al cerrar y volver a abrir
- permitir trabajo compartido entre varios integrantes
- limitar el borrado a los trazos del usuario actual

ACLARACION IMPORTANTE
---------------------
La aplicacion prioriza no perder cambios locales mientras el usuario esta
editando. Por esa razon, si existen modificaciones locales sin guardar, el
cliente evita reemplazarlas automaticamente con datos remotos.

Esto mejora la estabilidad de la experiencia, aunque implica una limitacion en
escenarios de edicion simultanea muy agresiva.

COMO DEJAR EL LIENZO EN BLANCO EN EL SERVIDOR
--------------------------------------------
Si el backend usa un archivo como:
		canvas_data.json

se puede limpiar con:
		echo '{"strokes":[]}' > canvas_data.json

o eliminando el archivo:
		rm -f canvas_data.json

Luego, al volver a cargar desde el cliente, el lienzo aparece vacio.

PRUEBAS REALIZADAS
------------------
- dibujo libre con click izquierdo
- goma con click derecho
- cambio de grosor con scroll
- cambio de color con teclas 1 a 9
- guardado manual en servidor
- recarga del estado al iniciar
- login de usuario
- restriccion de borrado por usuario
- persistencia del dibujo tras reiniciar la app

LIMITACIONES ACTUALES
---------------------
- no hay autenticacion real con password, solo nombre de usuario
- si dos usuarios ingresan el mismo nombre, se consideran el mismo autor
- la sincronizacion usa polling HTTP, no WebSockets
- no existe resolucion avanzada de conflictos por versionado fino
- el backend base guarda en archivo JSON y no en base de datos

MEJORAS POSIBLES
----------------
- autenticacion real de usuarios
- endpoint /clear desde el backend
- sincronizacion con WebSockets
- base de datos para persistencia
- indicador visual del tamano de goma/pincel
- bloqueo de nombres duplicados desde servidor

CONCLUSION
----------
El proyecto cumple los requerimientos principales de la consigna y presenta una
implementacion funcional de un lienzo colaborativo en tiempo real con Qt,
integrando dibujo, goma, guardado en VPS, carga remota y control basico por
usuario.

FECHA
-----
Abril 2026