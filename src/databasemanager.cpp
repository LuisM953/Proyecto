#include "databasemanager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QDateTime>

// ---------------------------------------------------------
// CONSTRUCTOR Y DESTRUCTOR
// ---------------------------------------------------------

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    // Establecer la ruta de la base de datos en un directorio con permisos de escritura (AppData)
    QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    QDir dir(path);
    if (!dir.exists()) {
        dir.mkpath(path);
    }

    m_dbPath = path + "/app_database.sqlite";
}

DatabaseManager::~DatabaseManager()
{
    closeDatabase();
}

// ---------------------------------------------------------
// GESTIÓN DE CONEXIÓN
// ---------------------------------------------------------

bool DatabaseManager::openDatabase()
{
    // Verificar si ya existe una conexión activa para reutilizarla
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        QSqlDatabase db = QSqlDatabase::database("qt_sql_default_connection");
        if (db.isOpen()) {
            m_database = db;
            return true;
        }
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(m_dbPath);

    if (!m_database.open()) {
        qCritical() << "Error al abrir la base de datos:" << m_database.lastError().text();
        return false;
    }

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
    QSqlDatabase::removeDatabase(connectionName);
}

// ---------------------------------------------------------
// INICIALIZACIÓN DE TABLAS
// ---------------------------------------------------------

bool DatabaseManager::createTables()
{
    QSqlQuery query;

    // 1. Tabla de Logs (Auditoría)
    QString logsTable = "CREATE TABLE IF NOT EXISTS logs ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                        "timestamp DATETIME, "
                        "category TEXT, "
                        "message TEXT)";

    if (!query.exec(logsTable)) {
        qCritical() << "Error creando tabla logs:" << query.lastError().text();
        return false;
    }

    // 2. Tabla de Usuarios (Autenticación)
    QString usersTable = "CREATE TABLE IF NOT EXISTS users ("
                         "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "username TEXT UNIQUE, "
                         "password TEXT, "
                         "role TEXT)";

    if (!query.exec(usersTable)) {
        qCritical() << "Error creando tabla users:" << query.lastError().text();
        return false;
    }

    // 3. Tabla de Dispositivos (Inventario)
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

    createDefaultUser();

    return true;
}

void DatabaseManager::createDefaultUser()
{
    QSqlQuery query("SELECT COUNT(*) FROM users");
    if (query.next() && query.value(0).toInt() == 0) {
        QSqlQuery insertQuery;
        insertQuery.prepare("INSERT INTO users (username, password, role) VALUES (:user, :pass, :role)");
        insertQuery.bindValue(":user", "admin");
        insertQuery.bindValue(":pass", "1234");
        insertQuery.bindValue(":role", "Administrator");
        insertQuery.exec();
    }
}

// ---------------------------------------------------------
// OPERACIONES CRUD Y UTILIDADES
// ---------------------------------------------------------

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

bool DatabaseManager::validateUser(const QString &username, const QString &password)
{
    QSqlQuery query;
    query.prepare("SELECT password FROM users WHERE username = :user");
    query.bindValue(":user", username);

    if (query.exec() && query.next()) {
        QString storedPass = query.value(0).toString();
        return (storedPass == password);
    }

    return false;
}

QSqlDatabase DatabaseManager::getDatabase() const
{
    return m_database;
}
