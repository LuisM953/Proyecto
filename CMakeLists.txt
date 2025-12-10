cmake_minimum_required(VERSION 3.16)

project(ProyectoFinal VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------
# CONFIGURACIÓN AUTOMÁTICA DE QT
# ---------------------------------------------------------
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------
# BUSCAR LIBRERÍAS DE QT
# ---------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Widgets Sql LinguistTools)

# ---------------------------------------------------------
# DEFINICIÓN DE ARCHIVOS (Con nuevas rutas)
# ---------------------------------------------------------
set(PROJECT_SOURCES
    # Archivos Fuente (.cpp) -> Carpeta src
    src/main.cpp
    src/mainwindow.cpp
    src/device.cpp
    src/user.cpp
    src/databasemanager.cpp
    src/devicemanager.cpp
    src/devicedialog.cpp
    src/registerdialog.cpp

    # Archivos de Cabecera (.h) -> Carpeta include
    include/mainwindow.h
    include/device.h
    include/user.h
    include/databasemanager.h
    include/devicemanager.h
    include/devicedialog.h
    include/registerdialog.h

    # Archivos de Diseño (.ui) -> Carpeta forms
    forms/mainwindow.ui
    forms/devicedialog.ui
    forms/registerdialog.ui

    # Recursos -> Carpeta resources
    resources/resources.qrc
)

# Archivo de traducción (asumiendo que lo moviste a una carpeta 'translations', si no, quita el prefijo)
set(TS_FILES translations/Proyecto_ALSE_es_CO.ts)

# ---------------------------------------------------------
# CREAR EL EJECUTABLE
# ---------------------------------------------------------
qt_add_executable(AppProyectoFinal
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${TS_FILES}
)

# ---------------------------------------------------------
# *** LA PARTE MÁGICA PARA QUE NO FALLE ***
# ---------------------------------------------------------

# 1. Le decimos a CMake que busque los .h dentro de 'include'.
#    Así, en tu código puedes seguir poniendo #include "mainwindow.h"
#    sin tener que escribir #include "../include/mainwindow.h"
target_include_directories(AppProyectoFinal PRIVATE include)

# 2. Le decimos a Qt que busque los diseños .ui dentro de 'forms'
set_target_properties(AppProyectoFinal PROPERTIES
    AUTOUIC_SEARCH_PATHS "forms"
)

# ---------------------------------------------------------
# CONFIGURACIÓN DE TRADUCCIONES
# ---------------------------------------------------------
qt_add_translations(AppProyectoFinal
    TS_FILES ${TS_FILES}
    QM_FILES_OUTPUT_VARIABLE QM_FILES
)

# ---------------------------------------------------------
# ENLAZAR LIBRERÍAS (LINKING)
# ---------------------------------------------------------
target_link_libraries(AppProyectoFinal PRIVATE
    Qt6::Widgets
    Qt6::Sql
)

# Configuración para Windows y macOS
set_target_properties(AppProyectoFinal PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)