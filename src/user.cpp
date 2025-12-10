#include "user.h"
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

// ---------------------------------------------------------
// CONSTRUCTOR Y DESTRUCTOR
// ---------------------------------------------------------

User::User(QObject *parent)
    : QObject(parent)
{
    clear();
}

User::~User()
{
}

// ---------------------------------------------------------
// GESTIÓN DE ESTADO INTERNO
// ---------------------------------------------------------

void User::clear()
{
    m_id = -1;
    m_username = "";
    m_role = "Guest";
    m_isLoggedIn = false;
}

// ---------------------------------------------------------
// LÓGICA DE AUTENTICACIÓN
// ---------------------------------------------------------

bool User::login(const QString &username, const QString &password)
{
    // Asegurar estado limpio antes de intentar login
    clear();

    QSqlQuery query;
    query.prepare("SELECT id, username, role FROM users WHERE username = :user AND password = :pass");
    query.bindValue(":user", username);
    query.bindValue(":pass", password);

    if (query.exec()) {
        if (query.next()) {
            m_id = query.value("id").toInt();
            m_username = query.value("username").toString();
            m_role = query.value("role").toString();
            m_isLoggedIn = true;

            emit userLoggedIn(m_username, m_role);
            return true;
        }
    } else {
        qCritical() << "Error en consulta de Login:" << query.lastError().text();
    }

    return false;
}

void User::logout()
{
    if (m_isLoggedIn) {
        clear();
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
    return (m_role.compare("admin", Qt::CaseInsensitive) == 0 ||
            m_role.compare("administrador", Qt::CaseInsensitive) == 0);
}

QString User::getUsername() const {
    return m_username;
}

QString User::getRole() const {
    return m_role;
}

int User::getId() const {
    return m_id;
}

void User::updateProfile(const QString &newRole) {
    m_role = newRole;
}
