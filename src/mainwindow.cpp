#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "devicedialog.h"
#include "devicemanager.h"
#include "databasemanager.h"
#include "registerdialog.h"
#include <QMessageBox>
#include <QSqlTableModel>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_model(nullptr)
{
    ui->setupUi(this);

    // Configuración visual inicial
    applyStyles();

    // Inicialización de la base de datos y modelo
    if (m_dbManager.openDatabase()) {
        setupDevicesTable();
    } else {
        ui->lblStatus->setText("Error: No hay conexión a BD");
        ui->lblStatus->setStyleSheet("color: red;");
    }

    // Iniciar siempre en la vista de Login
    ui->stackedWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (m_model) {
        delete m_model;
    }
}

// ---------------------------------------------------------
// CONFIGURACIÓN DE LA TABLA
// ---------------------------------------------------------

void MainWindow::setupDevicesTable()
{
    if (m_model == nullptr) {
        m_model = new QSqlTableModel(this, m_dbManager.getDatabase());
    }

    // Configuración del modelo
    m_model->setTable("devices");
    m_model->select();

    // Mapeo dinámico de columnas por nombre (independiente del orden en BD)
    int idxName = m_model->fieldIndex("name");
    int idxType = m_model->fieldIndex("type");
    int idxIp   = m_model->fieldIndex("ip_address");
    int idxCal  = m_model->fieldIndex("calibration");

    // Asignación de encabezados
    if(idxName != -1) m_model->setHeaderData(idxName, Qt::Horizontal, "Nombre");
    if(idxType != -1) m_model->setHeaderData(idxType, Qt::Horizontal, "Tipo");
    if(idxIp != -1)   m_model->setHeaderData(idxIp,   Qt::Horizontal, "Dirección IP");
    if(idxCal != -1)  m_model->setHeaderData(idxCal,  Qt::Horizontal, "Calibración");

    // Asignación a la vista
    ui->tableDevices->setModel(m_model);

    // Ocultar columnas internas (IDs)
    ui->tableDevices->setColumnHidden(m_model->fieldIndex("id"), true);
    ui->tableDevices->setColumnHidden(m_model->fieldIndex("user_id"), true);

    // Configuración visual de la tabla
    ui->tableDevices->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableDevices->setAlternatingRowColors(true);

    QHeaderView *header = ui->tableDevices->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
}

// ---------------------------------------------------------
// GESTIÓN DE SESIÓN (LOGIN / LOGOUT)
// ---------------------------------------------------------

void MainWindow::on_btnLogin_clicked()
{
    QString user = ui->inputUser->text();
    QString pass = ui->inputPassword->text();

    if (m_user.login(user, pass)) {
        // Registro de auditoría
        m_dbManager.insertLog("Login", "Usuario " + user + " inició sesión.");

        // Refrescar datos de la tabla sin reiniciar la configuración
        if (m_model) {
            m_model->setFilter("");
            m_model->select();
        }

        // Cambio de vista y actualización de UI
        ui->stackedWidget->setCurrentIndex(1);
        ui->lblWelcome->setText("Bienvenido, " + m_user.getUsername() +
                                " (" + m_user.getRole() + ")");

        ui->inputPassword->clear();
        ui->lblStatus->clear();

        // 1. Imprimir en consola qué rol detectó realmente (revisa la pestaña "Application Output")
        qDebug() << "ROL DETECTADO EN BD:" << m_user.getRole();

        // 2. Verificación Robusta: Convertimos a minúsculas para evitar errores de "Admin" vs "admin"
        QString roleLower = m_user.getRole().toLower().trimmed(); // trimmed() quita espacios extra

        if (m_user.isAdmin() || roleLower == "admin" || roleLower == "administrator") {
            ui->btnCreateUser->setVisible(true);
        } else {
            ui->btnCreateUser->setVisible(false);
        }
        // ---------------------------------------

    } else {
        ui->lblStatus->setText("Usuario o contraseña incorrectos");
        ui->lblStatus->setStyleSheet("color: red; font-weight: bold;");
    }
}

void MainWindow::on_btnLogout_clicked()
{
    m_user.logout();

    // Retorno al login y limpieza
    ui->stackedWidget->setCurrentIndex(0);
    ui->inputUser->clear();
    ui->inputPassword->clear();
    ui->lblStatus->setText("Sesión cerrada.");
    ui->lblStatus->setStyleSheet("color: green;");
    ui->btnCreateUser->setVisible(false);

    // Ocultar datos sensibles del modelo
    if(m_model) {
        m_model->setFilter("1=0");
    }
}

// ---------------------------------------------------------
// OPERACIONES CRUD (DISPOSITIVOS)
// ---------------------------------------------------------

void MainWindow::on_btnAddDevice_clicked()
{
    DeviceDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        Device *newDevice = dialog.getDeviceInfo();

        // Asignar dispositivo al usuario actual
        int currentUserId = m_user.getId();
        if (currentUserId <= 0) currentUserId = 1;

        newDevice->setUserId(currentUserId);

        DeviceManager devManager;
        if (devManager.addDevice(newDevice)) {
            QMessageBox::information(this, "Éxito", "Dispositivo guardado.");
            if (m_model) {
                m_model->select();
            }
        } else {
            QMessageBox::critical(this, "Error", "No se pudo guardar en la BD.");
        }
        delete newDevice;
    }
}

void MainWindow::on_btnDeleteDevice_clicked()
{
    QModelIndexList selectedRows = ui->tableDevices->selectionModel()->selectedRows();

    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Eliminar", "Selecciona el dispositivo a eliminar.");
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmar",
                                  "¿Borrar dispositivo seleccionado?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) return;

    int row = selectedRows.at(0).row();
    int deviceId = m_model->data(m_model->index(row, 0)).toInt();

    DeviceManager devManager;
    if (devManager.removeDevice(deviceId)) {
        m_model->select();
        QMessageBox::information(this, "Éxito", "Dispositivo eliminado.");
    } else {
        QMessageBox::critical(this, "Error", "No se pudo eliminar de la BD.");
    }
}

void MainWindow::on_btnEditDevice_clicked()
{
    QModelIndexList selectedRows = ui->tableDevices->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Editar", "Selecciona el dispositivo a editar.");
        return;
    }

    int row = selectedRows.at(0).row();

    // Recuperación de datos del modelo usando índices seguros
    int idxId = m_model->fieldIndex("id");
    int idxUser = m_model->fieldIndex("user_id");
    int idxName = m_model->fieldIndex("name");
    int idxType = m_model->fieldIndex("type");
    int idxIp = m_model->fieldIndex("ip_address");
    int idxCal = m_model->fieldIndex("calibration");

    int id = m_model->data(m_model->index(row, idxId)).toInt();
    int userId = m_model->data(m_model->index(row, idxUser)).toInt();
    QString name = m_model->data(m_model->index(row, idxName)).toString();
    QString type = m_model->data(m_model->index(row, idxType)).toString();
    QString ip = m_model->data(m_model->index(row, idxIp)).toString();

    double calib = 0.0;
    if(idxCal != -1) {
        calib = m_model->data(m_model->index(row, idxCal)).toDouble();
    }

    // Configuración del objeto temporal
    Device tempDev;
    tempDev.setId(id);
    tempDev.setUserId(userId);
    tempDev.setName(name);
    tempDev.setType(type);
    tempDev.setIp(ip);
    tempDev.setCalibration(calib);

    DeviceDialog dialog(this);
    dialog.setDeviceData(&tempDev);

    if (dialog.exec() == QDialog::Accepted) {
        Device *modifiedDev = dialog.getDeviceInfo();

        // Preservar integridad referencial
        modifiedDev->setId(id);
        modifiedDev->setUserId(userId);

        DeviceManager devManager;
        if (devManager.updateDevice(modifiedDev)) {
            m_model->select();
            QMessageBox::information(this, "Éxito", "Dispositivo actualizado.");
        } else {
            QMessageBox::critical(this, "Error", "No se pudo actualizar.");
        }
        delete modifiedDev;
    }
}

// ---------------------------------------------------------
// FUNCIONALIDADES ADICIONALES (BUSCAR, EXPORTAR, ADMIN)
// ---------------------------------------------------------

void MainWindow::on_txtSearch_textChanged(const QString &arg1)
{
    if (!m_model) return;

    if (arg1.isEmpty()) {
        m_model->setFilter("");
    } else {
        // Filtro SQL para búsqueda parcial en nombre o IP
        QString filter = QString("name LIKE '%%1%' OR ip_address LIKE '%%1%'").arg(arg1);
        m_model->setFilter(filter);
    }
    m_model->select();
}

void MainWindow::on_btnExport_clicked()
{
    if (!m_model || m_model->rowCount() == 0) {
        QMessageBox::warning(this, "Exportar", "No hay datos para exportar.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Guardar reporte",
                                                    QDir::homePath() + "/dispositivos.csv",
                                                    "Archivos CSV (*.csv);;Todos los archivos (*)");

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo crear el archivo.");
        return;
    }

    QTextStream out(&file);

    // Escribir encabezados
    QStringList headers;
    headers << "ID" << "Usuario_ID" << "Nombre" << "Tipo" << "IP" << "Calibracion";
    out << headers.join(";") << "\n";

    // Escribir filas
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < m_model->columnCount(); ++col) {
            QString data = m_model->data(m_model->index(row, col)).toString();
            // Sanitización básica para formato CSV
            data.replace(";", ",");
            rowData << data;
        }
        out << rowData.join(";") << "\n";
    }

    file.close();
    QMessageBox::information(this, "Éxito", "Datos exportados correctamente a:\n" + fileName);
}

void MainWindow::on_btnCreateUser_clicked()
{
    RegisterDialog dialog(this);
    dialog.exec();
}

// ---------------------------------------------------------
// ESTILOS VISUALES
// ---------------------------------------------------------

void MainWindow::applyStyles()
{
    QString style = R"(
        /* --- FONDO GENERAL --- */
        QMainWindow, QDialog {
            background-color: #2b2b2b;
            color: #ffffff;
            font-family: 'Segoe UI', sans-serif;
            font-size: 10pt;
        }

        /* --- BOTONES --- */
        QPushButton {
            background-color: #0d6efd;
            color: white;
            border: none;
            border-radius: 5px;
            padding: 6px 12px;
            font-size: 9pt;
            font-weight: bold;
            min-width: 80px;
        }
        QPushButton:hover {
            background-color: #0b5ed7;
        }
        QPushButton:pressed {
            background-color: #0a58ca;
        }

        /* Botón Exportar (Verde) */
        QPushButton#btnExport {
            background-color: #198754;
        }
        QPushButton#btnExport:hover {
            background-color: #157347;
        }

        /* Botones de acción negativa */
        QPushButton#btnDeleteDevice, QPushButton#btnLogout, QPushButton#btnCancel {
            background-color: #dc3545;
        }
        QPushButton#btnDeleteDevice:hover, QPushButton#btnLogout:hover, QPushButton#btnCancel:hover {
            background-color: #bb2d3b;
        }

        /* --- INPUTS --- */
        QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox {
            background-color: #3b3b3b;
            color: #ffffff;
            border: 1px solid #555;
            border-radius: 4px;
            padding: 5px;
        }
        QLineEdit:focus {
            border: 1px solid #0d6efd;
        }

        /* --- TABLA --- */
        QTableView {
            background-color: #3b3b3b;
            alternate-background-color: #444444;
            gridline-color: #555555;
            color: #ffffff;
            selection-background-color: #0d6efd;
            selection-color: white;
            border: 1px solid #555;
        }

        QHeaderView::section {
            background-color: #212121;
            color: white;
            padding: 5px;
            border: 1px solid #555;
            font-weight: bold;
        }

        /* --- LABELS --- */
        QLabel {
            color: #e0e0e0;
        }
        QLabel#lblWelcome {
            font-size: 14pt;
            font-weight: bold;
            color: #0d6efd;
        }
    )";

    qApp->setStyleSheet(style);
}
