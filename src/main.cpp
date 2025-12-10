#include "mainwindow.h"
#include "databasemanager.h"
#include "user.h"
#include "devicemanager.h"
#include "device.h"
#include <QApplication>
#include <QTranslator>    // <--- Aquí están de vuelta
#include <QLibraryInfo>   // <--- Aquí están de vuelta
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ---------------------------------------------------------
    // CÓDIGO DE TRADUCCIÓN (Lo que ya tenías o genera Qt por defecto)
    // ---------------------------------------------------------
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "AppProyectoFinal_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    // ---------------------------------------------------------

    MainWindow w;
    w.show();

    // ---------------------------------------------------------
    // ZONA DE PRUEBAS DE BASE DE DATOS Y USUARIO
    // ---------------------------------------------------------
    qDebug() << "\n--- INICIO DE PRUEBAS ---";

    // 1. Probar conexión a Base de Datos
    DatabaseManager dbManager;
    if (dbManager.openDatabase()) {
        dbManager.insertLog("System", "Aplicación iniciada con éxito");
        qDebug() << "Prueba DB: OK - Log insertado";
    } else {
        qDebug() << "Prueba DB: FALLO";
    }

    // 2. Probar Login de Usuario (Admin por defecto)
    User testUser;
    testUser.login("admin", "1234"); // Debería decir "Login exitoso"

    qDebug() << "--- FIN DE PRUEBAS ---\n";
    // ---------------------------------------------------------

    return a.exec();
}
