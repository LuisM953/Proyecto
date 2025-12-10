#include "user.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

// Constructor
User::User(QObject *parent)
    : QObject(parent)
{
    // Inicializamos todo limpio al crear el objeto
    clear();
}

// Destructor
User::~User()
{
}

// ---------------------------------------------------------
// FUNCIÓN CLEAR: Resetea todo a cero (Solución al fantasma)
// ---------------------------------------------------------
void User::clear()
{
    m_id = -1;              // ID inválido
    m_username = "";        // Sin nombre
    m_role = "Guest";       // Rol por defecto
    m_isLoggedIn = false;   // No logueado
}

// ---------------------------------------------------------
// LÓGICA DE AUTENTICACIÓN (CON BASE DE DATOS)
// ---------------------------------------------------------
bool User::login(const QString &username, const QString &password)
{
    // --- CAMBIO CLAVE AQUÍ ---
    // Eliminamos el chequeo de "si ya está logueado".
    // Siempre que llamamos a login, queremos intentar entrar con credenciales nuevas.

    // 1. Limpiamos cualquier rastro de usuario anterior por seguridad
    clear();

    qDebug() << "Intentando iniciar sesión (BD) para:" << username;

    QSqlQuery query;
    // Buscamos el usuario en la tabla 'users'
    query.prepare("SELECT id, username, role FROM users WHERE username = :user AND password = :pass");
    query.bindValue(":user", username);
    query.bindValue(":pass", password);

    if (query.exec()) {
        if (query.next()) {
            // --- LOGIN EXITOSO ---

            // 2. Guardamos TODOS los datos (incluyendo ID)
            m_id = query.value("id").toInt();
            m_username = query.value("username").toString();
            m_role = query.value("role").toString();
            m_isLoggedIn = true;

            qInfo() << "Login exitoso. ID:" << m_id << "Rol:" << m_role;

            // 3. Emitir señal actualizada
            emit userLoggedIn(m_username, m_role);
            return true;
        }
    } else {
        qCritical() << "Error en consulta de Login:" << query.lastError().text();
    }

    // --- LOGIN FALLIDO ---
    qWarning() << "Credenciales inválidas o error de BD.";
    // clear() ya se llamó al principio, pero no está de más asegurar
    return false;
}

void User::logout()
{
    // Solo cerramos si hay alguien dentro, aunque clear() es seguro llamarlo siempre
    if (m_isLoggedIn) {
        qInfo() << "Cerrando sesión de:" << m_username;
        clear(); // <--- Borra ID, Nombre y estado
        emit userLoggedOut();
    }
}

// ---------------------------------------------------------
// GETTERS Y SETTERS
// ---------------------------------------------------------

bool User::isLoggedIn() const {
    return m_isLoggedIn;
}

bool User::isAdmin() const {
    // Ajusta la cadena "admin" o "Administrator" según lo que tengas en tu BD
    return (m_role == "admin" || m_role == "Administrador");
}

QString User::getUsername() const {
    return m_username;
}

QString User::getRole() const {
    return m_role;
}

// Getter del ID
int User::getId() const {
    return m_id;
}

void User::updateProfile(const QString &newRole) {
    m_role = newRole;
}
