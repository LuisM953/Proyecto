#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QString>

/**
 * @brief Clase que modela un dispositivo físico o virtual dentro del sistema.
 *
 * Esta clase cumple dos funciones principales:
 * 1. Actúa como un objeto de transferencia de datos (DTO) para la base de datos (guardando ID, nombre, IP, etc.).
 * 2. Gestiona la lógica de conexión (simulada o real) y comunicación a través de señales y slots.
 */
class Device : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la clase Device.
     * @param parent Puntero al objeto padre para la gestión de memoria de Qt.
     */
    explicit Device(QObject *parent = nullptr);

    /**
     * @brief Destructor de la clase.
     */
    ~Device();

    // ---------------------------------------------------------
    // GETTERS Y SETTERS (Atributos del Modelo de Datos)
    // ---------------------------------------------------------

    /**
     * @brief Establece el identificador único del dispositivo.
     * @param id ID numérico generado por la base de datos.
     */
    void setId(int id);

    /**
     * @brief Obtiene el identificador único del dispositivo.
     * @return ID del dispositivo.
     */
    int getId() const;

    /**
     * @brief Asigna el dispositivo a un usuario específico.
     * @param userId ID del usuario propietario.
     */
    void setUserId(int userId);

    /**
     * @brief Obtiene el ID del usuario propietario del dispositivo.
     * @return ID del usuario.
     */
    int getUserId() const;

    /**
     * @brief Establece el nombre legible del dispositivo.
     * @param name Nombre del dispositivo (ej. "Sensor Sala 1").
     */
    void setName(const QString &name);

    /**
     * @brief Obtiene el nombre del dispositivo.
     * @return Cadena con el nombre.
     */
    QString getName() const;

    /**
     * @brief Establece la categoría o tipo de dispositivo.
     * @param type Tipo (ej. "Sensor", "Actuador", "PLC").
     */
    void setType(const QString &type);

    /**
     * @brief Obtiene el tipo de dispositivo.
     * @return Cadena con el tipo.
     */
    QString getType() const;

    /**
     * @brief Establece la dirección IP del dispositivo.
     * @param ip Dirección IP en formato string (ej. "192.168.1.50").
     */
    void setIp(const QString &ip);

    /**
     * @brief Obtiene la dirección IP del dispositivo.
     * @return Cadena con la IP.
     */
    QString getIp() const;

    /**
     * @brief Establece el valor de calibración o ajuste.
     * @param value Valor numérico de calibración.
     */
    void setCalibration(double value);

    /**
     * @brief Obtiene el valor de calibración actual.
     * @return Valor double de la calibración.
     */
    double getCalibration() const;

    // ---------------------------------------------------------
    // LÓGICA DE CONEXIÓN
    // ---------------------------------------------------------

    /**
     * @brief Intenta establecer conexión con el dispositivo.
     * Utiliza la dirección IP almacenada para realizar la conexión.
     * @return true si la conexión fue exitosa, false en caso contrario.
     */
    bool connectToDevice();

    /**
     * @brief Cierra la conexión activa con el dispositivo.
     */
    void disconnectDevice();

    /**
     * @brief Envía datos al dispositivo conectado.
     * @param data Cadena de texto con los datos o comandos a enviar.
     */
    void sendData(const QString &data);

    /**
     * @brief Verifica el estado actual de la conexión.
     * @return true si está conectado, false si está desconectado.
     */
    bool isConnected() const;

signals:
    /**
     * @brief Señal emitida cuando la conexión se establece exitosamente.
     */
    void deviceConnected();

    /**
     * @brief Señal emitida cuando el dispositivo se desconecta.
     */
    void deviceDisconnected();

    /**
     * @brief Señal emitida cuando el dispositivo envía datos al sistema.
     * @param data Los datos recibidos.
     */
    void dataReceived(const QString &data);

    /**
     * @brief Señal emitida cuando ocurre un error en la comunicación.
     * @param msg Descripción del error.
     */
    void errorOccurred(const QString &msg);

    /**
     * @brief Señal informativa sobre cambios de estado.
     * @param status Nuevo estado (ej. "Conectando...", "Listo").
     */
    void statusChanged(const QString &status);

private:
    // --- VARIABLES DE ALMACENAMIENTO DE DATOS ---
    int m_id;             /**< ID único en la base de datos. */
    int m_userId;         /**< ID del usuario dueño. */
    QString m_name;       /**< Nombre descriptivo. */
    QString m_type;       /**< Tipo de dispositivo. */
    QString m_ip;         /**< Dirección IP. */
    double m_calibration; /**< Valor de ajuste de calibración. */

    // --- VARIABLES DE ESTADO ---
    bool m_isConnected;   /**< Bandera de estado de conexión. */
};

#endif // DEVICE_H
