#include "devicemanager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

DeviceManager::DeviceManager(QObject *parent)
    : QObject(parent)
{
}

// ---------------------------------------------------------
// CREAR (INSERT)
// ---------------------------------------------------------

bool DeviceManager::addDevice(Device *device)
{
    if (!device) return false;

    QSqlQuery query;
    query.prepare("INSERT INTO devices (user_id, name, type, ip_address, calibration) "
                  "VALUES (:user, :name, :type, :ip, :cal)");

    query.bindValue(":user", device->getUserId());
    query.bindValue(":name", device->getName());
    query.bindValue(":type", device->getType());
    query.bindValue(":ip", device->getIp());
    query.bindValue(":cal", device->getCalibration());

    if (!query.exec()) {
        qCritical() << "Error al agregar dispositivo:" << query.lastError().text();
        return false;
    }

    emit deviceListChanged();
    return true;
}

// ---------------------------------------------------------
// LEER (SELECT)
// ---------------------------------------------------------

QList<Device*> DeviceManager::getDevicesByUser(int userId)
{
    QList<Device*> list;
    QSqlQuery query;

    query.prepare("SELECT id, name, type, ip_address, calibration FROM devices WHERE user_id = :uid");
    query.bindValue(":uid", userId);

    if (query.exec()) {
        while (query.next()) {
            Device *dev = new Device();

            dev->setId(query.value("id").toInt());
            dev->setUserId(userId);
            dev->setName(query.value("name").toString());
            dev->setType(query.value("type").toString());
            dev->setIp(query.value("ip_address").toString());
            dev->setCalibration(query.value("calibration").toDouble());

            list.append(dev);
        }
    } else {
        qCritical() << "Error recuperando dispositivos:" << query.lastError().text();
    }

    return list;
}

// ---------------------------------------------------------
// ACTUALIZAR (UPDATE)
// ---------------------------------------------------------

bool DeviceManager::updateDevice(Device *device)
{
    if (!device || device->getId() == -1) return false;

    QSqlQuery query;
    query.prepare("UPDATE devices SET name = :name, type = :type, "
                  "ip_address = :ip, calibration = :cal WHERE id = :id");

    query.bindValue(":name", device->getName());
    query.bindValue(":type", device->getType());
    query.bindValue(":ip", device->getIp());
    query.bindValue(":cal", device->getCalibration());
    query.bindValue(":id", device->getId());

    if (!query.exec()) {
        qCritical() << "Error actualizando dispositivo:" << query.lastError().text();
        return false;
    }

    emit deviceListChanged();
    return true;
}

// ---------------------------------------------------------
// BORRAR (DELETE)
// ---------------------------------------------------------

bool DeviceManager::removeDevice(int deviceId)
{
    QSqlQuery query;
    query.prepare("DELETE FROM devices WHERE id = :id");
    query.bindValue(":id", deviceId);

    if (!query.exec()) {
        qCritical() << "Error eliminando dispositivo:" << query.lastError().text();
        return false;
    }

    emit deviceListChanged();
    return true;
}
