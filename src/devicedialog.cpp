#include "devicedialog.h"
#include "ui_devicedialog.h"
#include <QMessageBox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

DeviceDialog::DeviceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeviceDialog),
    m_device(nullptr) // Inicializamos el puntero en nulo
{
    ui->setupUi(this);
    // 1. Validación de IP (Formato: num.num.num.num)
    // Esta expresión regular permite números del 0-999 separados por puntos.
    // (Es una versión simplificada pero funcional).
    QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
    QRegularExpression ipRegex("^" + ipRange + "\\." + ipRange + "\\." + ipRange + "\\." + ipRange + "$");

    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->txtIp->setValidator(ipValidator);

    // Opcional: Poner un texto de ayuda (placeholder)
    ui->txtIp->setPlaceholderText("Ej: 192.168.0.1");

    // 2. Validación de Calibración (Límites)
    // Configura el mínimo y máximo permitido
    ui->spinCalibration->setRange(-100.0, 100.0);

    // Configura cuántos decimales quieres ver
    ui->spinCalibration->setDecimals(2);

    // Configura de cuánto en cuánto sube la flechita
    ui->spinCalibration->setSingleStep(0.1);

    // ----------------------------------
}

DeviceDialog::~DeviceDialog()
{
    delete ui;
    // No borramos m_device aquí, la ventana principal se encargará de él.
}

// ---------------------------------------------------------
// BOTÓN GUARDAR
// ---------------------------------------------------------
void DeviceDialog::on_btnSave_clicked()
{
    // 1. Validar que no haya campos vacíos
    if(ui->txtName->text().isEmpty() || ui->txtIp->text().isEmpty()) {
        QMessageBox::warning(this, "Aviso", "El nombre y la IP son obligatorios.");
        return;
    }

    // 2. Crear el objeto Device con los datos de la interfaz
    // Si ya existía uno previo (por si le dieron guardar dos veces), lo limpiamos
    if (m_device) {
        delete m_device;
    }

    m_device = new Device();

    // Usamos los nombres CORRECTOS de tu interfaz:
    m_device->setName(ui->txtName->text());
    m_device->setType(ui->comboType->currentText()); // ComboBox
    m_device->setIp(ui->txtIp->text());
    m_device->setCalibration(ui->spinCalibration->value());

    // 3. Cerrar el diálogo indicando ÉXITO
    accept();
}

// ---------------------------------------------------------
// BOTÓN CANCELAR
// ---------------------------------------------------------
void DeviceDialog::on_btnCancel_clicked()
{
    reject();
}

// ---------------------------------------------------------
// OBTENER INFORMACIÓN (Para que el Main lo recoja)
// ---------------------------------------------------------
Device* DeviceDialog::getDeviceInfo()
{
    // Retornamos el objeto creado en on_btnSave_clicked
    return m_device;
}

// ---------------------------------------------------------
// CARGAR INFORMACIÓN (Para Editar)
// ---------------------------------------------------------
void DeviceDialog::setDeviceData(Device *device)
{
    if (!device) return;

    // AQUÍ ESTABA EL ERROR: Debemos usar los nombres reales de tu UI

    // 1. Nombre
    ui->txtName->setText(device->getName());

    // 2. Tipo (ComboBox)
    ui->comboType->setCurrentText(device->getType());

    // 3. IP
    ui->txtIp->setText(device->getIp());

    // 4. Calibración
    ui->spinCalibration->setValue(device->getCalibration());
}
