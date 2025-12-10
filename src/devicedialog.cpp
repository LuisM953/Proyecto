#include "devicedialog.h"
#include "ui_devicedialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// ---------------------------------------------------------
// CONSTRUCTOR Y DESTRUCTOR
// ---------------------------------------------------------

DeviceDialog::DeviceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDialog),
    m_device(nullptr)
{
    ui->setupUi(this);

    // Configuración de validación para dirección IP (IPv4)
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression ipRegex("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");

    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->txtIp->setValidator(ipValidator);
    ui->txtIp->setPlaceholderText("Ej: 192.168.0.1");

    // Configuración del campo de calibración
    ui->spinCalibration->setRange(-100.0, 100.0);
    ui->spinCalibration->setDecimals(2);
    ui->spinCalibration->setSingleStep(0.1);
}

DeviceDialog::~DeviceDialog()
{
    delete ui;
    // Nota: m_device no se elimina aquí porque se transfiere su propiedad a MainWindow
}

// ---------------------------------------------------------
// BOTONES DE ACCIÓN
// ---------------------------------------------------------

void DeviceDialog::on_btnSave_clicked()
{
    // Validar campos obligatorios
    if(ui->txtName->text().isEmpty() || ui->txtIp->text().isEmpty()) {
        QMessageBox::warning(this, "Aviso", "El nombre y la IP son obligatorios.");
        return;
    }

    // Limpiar objeto previo si existe para evitar fugas en reintentos
    if (m_device) {
        delete m_device;
    }

    // Crear nuevo objeto con los datos del formulario
    m_device = new Device();
    m_device->setName(ui->txtName->text());
    m_device->setType(ui->comboType->currentText());
    m_device->setIp(ui->txtIp->text());
    m_device->setCalibration(ui->spinCalibration->value());

    accept();
}

void DeviceDialog::on_btnCancel_clicked()
{
    reject();
}

// ---------------------------------------------------------
// MÉTODOS DE TRANSFERENCIA DE DATOS
// ---------------------------------------------------------

Device* DeviceDialog::getDeviceInfo()
{
    return m_device;
}

void DeviceDialog::setDeviceData(Device *device)
{
    if (!device) return;

    // Cargar datos del dispositivo en la interfaz para edición
    ui->txtName->setText(device->getName());
    ui->comboType->setCurrentText(device->getType());
    ui->txtIp->setText(device->getIp());
    ui->spinCalibration->setValue(device->getCalibration());
}
