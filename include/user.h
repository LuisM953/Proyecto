#ifndef USER_H
#define USER_H

#include <QObject>
#include <QString>

/**
 * @brief Clase que gestiona la sesión, roles y autenticación del usuario actual.
 *
 * Mantiene en memoria el estado del usuario que ha iniciado sesión (ID, nombre, rol).
 * Se encarga de validar credenciales contra la base de datos y emitir señales
 * cuando el estado de la sesión cambia (login/logout).
 */
class User : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la clase User.
     * Inicializa el estado como "desconectado" (Guest).
     * @param parent Objeto padre opcional.
     */
    explicit User(QObject *parent = nullptr);

    /**
     * @brief Destructor de la clase.
     */
    ~User();

    /**
     * @brief Intenta iniciar sesión con las credenciales proporcionadas.
     *
     * Consulta la base de datos para verificar usuario y contraseña. Si es correcto,
     * carga los datos (ID, Rol) en memoria.
     *
     * @param username Nombre de usuario.
     * @param password Contraseña.
     * @return true si las credenciales son válidas, false en caso contrario.
     */
    bool login(const QString &username, const QString &password);

    /**
     * @brief Cierra la sesión actual.
     * Limpia los datos del usuario en memoria y emite la señal userLoggedOut.
     */
    void logout();

    /**
     * @brief Resetea los datos internos del objeto a valores por defecto.
     * Establece ID=-1, Rol="Guest" y loggedIn=false. Es útil para asegurar
     * que no queden datos residuales de sesiones anteriores.
     */
    void clear();

    // ---------------------------------------------------------
    // GETTERS (Acceso a la información del usuario)
    // ---------------------------------------------------------

    /**
     * @brief Verifica si hay una sesión activa.
     * @return true si el usuario está logueado.
     */
    bool isLoggedIn() const;

    /**
     * @brief Verifica si el usuario actual tiene privilegios de administrador.
     * @return true si el rol es "admin" o "Administrator".
     */
    bool isAdmin() const;

    /**
     * @brief Obtiene el nombre de usuario de la sesión actual.
     * @return Cadena con el username.
     */
    QString getUsername() const;

    /**
     * @brief Obtiene el rol del usuario actual (ej. "admin", "user", "guest").
     * @return Cadena con el rol.
     */
    QString getRole() const;

    /**
     * @brief Obtiene el ID único de base de datos del usuario.
     * Fundamental para relacionar al usuario con sus dispositivos (Foreign Key).
     * @return ID numérico (retorna -1 si no hay sesión).
     */
    int getId() const;

    /**
     * @brief Actualiza el rol del usuario en memoria (no en BD).
     * @param newRole El nuevo rol a asignar.
     */
    void updateProfile(const QString &newRole);

signals:
    /**
     * @brief Señal emitida cuando un usuario inicia sesión exitosamente.
     * @param username Nombre del usuario.
     * @param role Rol del usuario.
     */
    void userLoggedIn(const QString &username, const QString &role);

    /**
     * @brief Señal emitida cuando se cierra la sesión.
     */
    void userLoggedOut();

private:
    // --- ESTADO DEL USUARIO ---
    int m_id;             /**< ID único en la tabla 'users' de la BD. */
    bool m_isLoggedIn;    /**< Bandera de estado de sesión. */
    QString m_username;   /**< Nombre de usuario actual. */
    QString m_role;       /**< Rol o nivel de permisos. */
};

#endif // USER_H
