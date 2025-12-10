#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

/**
 * @brief Clase responsable de gestionar la conexión y operaciones directas con la base de datos SQLite.
 *
 * Esta clase maneja la apertura del archivo de base de datos, la creación de tablas iniciales
 * y provee funciones utilitarias para el registro de logs y validación básica.
 */
class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la clase DatabaseManager.
     * @param parent Puntero al objeto padre (opcional) para la gestión de memoria de Qt.
     */
    explicit DatabaseManager(QObject *parent = nullptr);

    /**
     * @brief Destructor de la clase.
     * Se encarga de cerrar la conexión si está abierta al destruir el objeto.
     */
    ~DatabaseManager();

    /**
     * @brief Abre la conexión con la base de datos SQLite.
     *
     * Si el archivo de base de datos no existe, lo crea. También verifica e inicializa
     * las tablas necesarias llamando a createTables().
     *
     * @return true si la conexión fue exitosa y las tablas están listas.
     * @return false si hubo un error al abrir la BD o crear tablas.
     */
    bool openDatabase();

    /**
     * @brief Cierra la conexión actual con la base de datos.
     */
    void closeDatabase();

    /**
     * @brief Inserta un registro en la tabla de auditoría (logs).
     * @param category Categoría del evento (ej. "Login", "Error", "Sistema").
     * @param message Descripción detallada del evento.
     * @return true si la inserción fue correcta, false en caso contrario.
     */
    bool insertLog(const QString &category, const QString &message);

    /**
     * @brief Valida si un usuario y contraseña existen en la base de datos.
     * @param username Nombre de usuario a verificar.
     * @param password Contraseña del usuario.
     * @return true si las credenciales son válidas, false si no coinciden.
     */
    bool validateUser(const QString &username, const QString &password);

    /**
     * @brief Obtiene el objeto de conexión a la base de datos.
     * Utilizado por los modelos (QSqlTableModel) para poblar las vistas.
     * @return Objeto QSqlDatabase con la conexión configurada.
     */
    QSqlDatabase getDatabase() const;

private:
    /**
     * @brief Objeto interno de Qt que maneja la conexión SQL.
     */
    QSqlDatabase m_database;

    /**
     * @brief Ruta al archivo físico de la base de datos en el disco.
     */
    QString m_dbPath;

    /**
     * @brief Crea las tablas necesarias (users, devices, logs) si no existen.
     * @return true si todas las tablas se verificaron/crearon correctamente.
     */
    bool createTables();

    /**
     * @brief Inserta un usuario 'admin' por defecto.
     * Esta función se ejecuta solo si la tabla de usuarios está vacía para evitar bloqueos.
     */
    void createDefaultUser();
};

#endif // DATABASEMANAGER_H
