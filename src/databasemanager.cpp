#include "databasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDateTime>

// Constructor: Configura la ruta de la base de datos
DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    // Usamos una ubicación estándar del sistema para guardar datos de la aplicación
    // Esto evita problemas de permisos en Windows/Linux/macOS
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Asegurarnos de que el directorio existe
    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }

    // Definir la ruta completa del archivo de base de datos
    m_dbPath = path + "/app_database.sqlite";
    qDebug() << "Ruta de la base de datos:" << m_dbPath;
}

// Destructor: Cierra la conexión al destruir el objeto
DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

// ---------------------------------------------------------
// GESTIÓN DE LA CONEXIÓN
// ---------------------------------------------------------

bool DatabaseManager::openDatabase()
{
    // Verificar si ya está conectada
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
        if (db.isOpen()) {
            return true;
        }
    }

    // Configurar la conexión SQLite
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(m_dbPath);

    if (!db.open()) {
        qCritical() << "Error al abrir la base de datos:" << db.lastError().text();
        return false;
    }

    qDebug() << "Base de datos conectada exitosamente.";

    // Al abrir, nos aseguramos de que las tablas existan
    return createTables();
}

void DatabaseManager::closeDatabase()
{
    QString connectionName;
    {
        QSqlDatabase db = QSqlDatabase::database();
        connectionName = db.connectionName();
        if (db.isOpen()) {
            db.close();
        }
    }
    // Nota: QSqlDatabase::removeDatabase debe llamarse fuera del alcance donde se usa 'db'
    QSqlDatabase::removeDatabase(connectionName);
    qDebug() << "Base de datos desconectada.";
}

// ---------------------------------------------------------
// CREACIÓN DE ESTRUCTURA
// ---------------------------------------------------------

bool DatabaseManager::createTables()
{
    QSqlQuery query;

    // 1. Tabla LOGS (Sin cambios)
    QString logsTable = "CREATE TABLE IF NOT EXISTS logs ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "timestamp DATETIME, "
                        "category TEXT, "
                        "message TEXT)";

    if (!query.exec(logsTable)) {
        qCritical() << "Error creando tabla logs:" << query.lastError().text();
        return false;
    }

    // 2. Tabla USERS (Sin cambios)
    QString usersTable = "CREATE TABLE IF NOT EXISTS users ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "username TEXT UNIQUE, "
                         "password TEXT, "
                         "role TEXT)";

    if (!query.exec(usersTable)) {
        qCritical() << "Error creando tabla users:" << query.lastError().text();
        return false;
    }

    // 3. NUEVA TABLA DEVICES (Requerimiento del Modelo ER)
    // Campos: id, user_id (FK), name, type, ip_address, calibration
    QString devicesTable = "CREATE TABLE IF NOT EXISTS devices ("
                           "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                           "user_id INTEGER, "
                           "name TEXT, "
                           "type TEXT, "
                           "ip_address TEXT, "
                           "calibration REAL, "
                           "FOREIGN KEY(user_id) REFERENCES users(id))";

    if (!query.exec(devicesTable)) {
        qCritical() << "Error creando tabla devices:" << query.lastError().text();
        return false;
    }

    // Crear usuario admin por defecto
    createDefaultUser();

    return true;
}

void DatabaseManager::createDefaultUser()
{
    // Verifica si la tabla usuarios está vacía
    QSqlQuery query("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 0) {
        // Insertar admin por defecto
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO users (username, password, role) VALUES (:user, :pass, :role)");
        insertQuery.bindValue(":user", "admin");
        insertQuery.bindValue(":pass", "1234"); // En producción usar hash!
        insertQuery.bindValue(":role", "Administrador");
        insertQuery.exec();
        qDebug() << "Usuario admin por defecto creado.";
    }
}

// ---------------------------------------------------------
// MÉTODOS DE OPERACIÓN (CRUD)
// ---------------------------------------------------------

// Ejemplo: Insertar un registro en el log
bool DatabaseManager::insertLog(const QString &category, const QString &message)
{
    QSqlQuery query;
    query.prepare("INSERT INTO logs (timestamp, category, message) VALUES (:time, :cat, :msg)");

    query.bindValue(":time", QDateTime::currentDateTime());
    query.bindValue(":cat", category);
    query.bindValue(":msg", message);

    if (!query.exec()) {
        qWarning() << "Error insertando log:" << query.lastError().text();
        return false;
    }
    return true;
}

// Ejemplo: Validar usuario (conecta con la lógica de User.cpp)
bool DatabaseManager::validateUser(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = :user");
    query.bindValue(":user", username);

    if (query.exec() && query.next()) {
        QString storedPass = query.value(0).toString();
        // Comparación simple (en real usarías hash verification)
        return (storedPass == password);
    }

    return false;
}
QSqlDatabase DatabaseManager::getDatabase() const
{
    return m_database; // Asumiendo que tu variable interna se llama m_database
}
