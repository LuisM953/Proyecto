#include "device.h"
#include <QDebug>

// ---------------------------------------------------------
// CONSTRUCTOR Y DESTRUCTOR
// ---------------------------------------------------------

Device::Device(QObject *parent)
    : QObject(parent)
    , m_id(-1)
    , m_userId(-1)
    , m_name("Nuevo Dispositivo")
    , m_type("Genérico")
    , m_ip("192.168.1.1")
    , m_calibration(0.0)
    , m_isConnected(false)
{
}

Device::~Device()
{
    disconnectDevice();
}

// ---------------------------------------------------------
// GETTERS Y SETTERS
// ---------------------------------------------------------

void Device::setId(int id) { m_id = id; }
int Device::getId() const { return m_id; }

void Device::setUserId(int userId) { m_userId = userId; }
int Device::getUserId() const { return m_userId; }

void Device::setName(const QString &nombre) { m_name = nombre; }
QString Device::getName() const { return m_name; }

void Device::setType(const QString &tipo) { m_type = tipo; }
QString Device::getType() const { return m_type; }

void Device::setIp(const QString &ip) { m_ip = ip; }
QString Device::getIp() const { return m_ip; }

void Device::setCalibration(double valor) { m_calibration = valor; }
double Device::getCalibration() const { return m_calibration; }

// ---------------------------------------------------------
// LÓGICA DE CONEXIÓN
// ---------------------------------------------------------

bool Device::connectToDevice()
{
    if (m_isConnected) return true;

    qDebug() << "Haciendo Ping a" << m_ip << "...";

    if (!m_ip.isEmpty()) {
        m_isConnected = true;
        emit deviceConnected();
        emit statusChanged("Conectado a " + m_ip);
        return true;
    }

    emit errorOccurred("IP inválida");
    return false;
}

void Device::disconnectDevice()
{
    if (!m_isConnected) return;

    m_isConnected = false;
    emit deviceDisconnected();
    emit statusChanged("Desconectado de " + m_ip);
}

void Device::sendData(const QString &data)
{
    if (!m_isConnected) {
        emit errorOccurred("Dispositivo desconectado");
        return;
    }

    qDebug() << "Enviando configuración a" << m_ip << ":" << data;
}
