#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H

#include <QObject>
#include <QList>
#include "device.h"

/**
 * @brief Clase controladora encargada de la lógica de negocio y gestión de datos de los dispositivos.
 *
 * Actúa como intermediaria entre la interfaz gráfica y la base de datos (Data Access Object).
 * Implementa las operaciones CRUD (Create, Read, Update, Delete) completas.
 */
class DeviceManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructor de la clase DeviceManager.
     * @param parent Objeto padre opcional.
     */
    explicit DeviceManager(QObject *parent = nullptr);

    /**
     * @brief Inserta un nuevo dispositivo en la base de datos.
     *
     * Toma los datos del objeto Device proporcionado (nombre, tipo, ip, calibración, userId)
     * y ejecuta la sentencia SQL INSERT.
     *
     * @param device Puntero al objeto Device con la información a guardar.
     * @return true si la inserción en la BD fue exitosa, false si hubo error SQL.
     */
    bool addDevice(Device *device);

    /**
     * @brief Recupera la lista de dispositivos asociados a un usuario específico.
     *
     * Ejecuta un SELECT filtrando por el ID de usuario.
     *
     * @warning La función crea nuevos objetos Device en el heap (memoria dinámica).
     * Es responsabilidad absoluta de quien llama a esta función eliminar (delete)
     * los punteros de la lista para evitar fugas de memoria.
     *
     * @param userId El ID del usuario del cual se quieren obtener los dispositivos.
     * @return Una lista (QList) de punteros a objetos Device.
     */
    QList<Device*> getDevicesByUser(int userId);

    /**
     * @brief Actualiza la información de un dispositivo existente.
     *
     * Busca el registro por ID y actualiza sus campos (nombre, tipo, ip, calibración).
     *
     * @param device Puntero al objeto Device que contiene los datos modificados y el ID original.
     * @return true si la actualización fue correcta, false si falló.
     */
    bool updateDevice(Device *device);

    /**
     * @brief Elimina un dispositivo de la base de datos permanentemente.
     *
     * @param deviceId El identificador único (ID) del dispositivo a borrar.
     * @return true si se eliminó correctamente, false si hubo error.
     */
    bool removeDevice(int deviceId);

signals:
    /**
     * @brief Señal emitida cuando ocurre cualquier cambio en la lista de dispositivos.
     * Útil para notificar a la interfaz que debe refrescar la tabla.
     */
    void deviceListChanged();
};

#endif // DEVICEMANAGER_H
