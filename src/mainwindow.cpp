#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "devicedialog.h"
#include "devicemanager.h"
#include "databasemanager.h"
#include "registerdialog.h" // <--- NECESARIO para btnCreateUser
#include <QMessageBox>
#include <QSqlTableModel>
#include <QFile>        // <--- NECESARIO para Exportar
#include <QTextStream>  // <--- NECESARIO para Exportar
#include <QFileDialog>  // <--- NECESARIO para Exportar
#include <QDir>         // <--- NECESARIO para Exportar

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_model(nullptr)
{
    ui->setupUi(this);

    // 1. Aplicar estilos visuales al iniciar
    applyStyles();

    // 2. Inicializar Base de Datos
    if (m_dbManager.openDatabase()) {
        // Configuramos la tabla UNA sola vez al inicio
        setupDevicesTable();
    } else {
        ui->lblStatus->setText("Error: No hay conexión a BD");
        ui->lblStatus->setStyleSheet("color: red;");
    }

    // 3. Asegurarnos de empezar en la página de Login
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

    // 1. Asignar tabla y cargar datos
    m_model->setTable("devices");
    m_model->select();

    // 2. Usar fieldIndex para encontrar las columnas sin importar el orden
    int idxName = m_model->fieldIndex("name");
    int idxType = m_model->fieldIndex("type");
    int idxIp   = m_model->fieldIndex("ip_address");

    // Verificamos si existe calibración
    int idxCal  = m_model->fieldIndex("calibration");

    // 3. Asignar nombres bonitos (Headers)
    if(idxName != -1) m_model->setHeaderData(idxName, Qt::Horizontal, "Nombre");
    if(idxType != -1) m_model->setHeaderData(idxType, Qt::Horizontal, "Tipo");
    if(idxIp != -1)   m_model->setHeaderData(idxIp,   Qt::Horizontal, "Dirección IP");
    if(idxCal != -1)  m_model->setHeaderData(idxCal,  Qt::Horizontal, "Calibración");

    // 4. Asignar modelo a la vista
    ui->tableDevices->setModel(m_model);

    // 5. Ocultar columnas internas (ID y User_ID)
    ui->tableDevices->setColumnHidden(m_model->fieldIndex("id"), true);
    ui->tableDevices->setColumnHidden(m_model->fieldIndex("user_id"), true);

    // 6. Configuración visual
    ui->tableDevices->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableDevices->setAlternatingRowColors(true);

    // Ajuste de tamaño (Estirar columnas)
    QHeaderView *header = ui->tableDevices->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);
}

// ---------------------------------------------------------
// LOGIN
// ---------------------------------------------------------
void MainWindow::on_btnLogin_clicked()
{
    QString user = ui->inputUser->text();
    QString pass = ui->inputPassword->text();

    if (m_user.login(user, pass)) {
        // --- ÉXITO ---
        m_dbManager.insertLog("Login", "Usuario " + user + " inició sesión.");

        // --- CORRECCIÓN CLAVE: NO llamamos a setTable aquí ---
        if (m_model) {
            m_model->setFilter(""); // Limpiar filtros viejos
            m_model->select();      // Refrescar datos
        }
        // ----------------------------------------------------

        ui->stackedWidget->setCurrentIndex(1);

        ui->lblWelcome->setText("Bienvenido, " + m_user.getUsername() +
                                " (" + m_user.getRole() + ")");

        ui->inputPassword->clear();
        ui->lblStatus->clear();

        // Seguridad de botones
        if (m_user.isAdmin()) {
            ui->btnCreateUser->setVisible(true);
        } else {
            ui->btnCreateUser->setVisible(false);
        }

    } else {
        // --- FALLO ---
        ui->lblStatus->setText("Usuario o contraseña incorrectos");
        ui->lblStatus->setStyleSheet("color: red; font-weight: bold;");
    }
}

// ---------------------------------------------------------
// LOGOUT
// ---------------------------------------------------------
void MainWindow::on_btnLogout_clicked()
{
    // 1. Borrar la sesión del objeto User
    m_user.logout();

    // 2. Volver al Login
    ui->stackedWidget->setCurrentIndex(0);

    // 3. Limpiar UI
    ui->inputUser->clear();
    ui->inputPassword->clear();
    ui->lblStatus->setText("Sesión cerrada.");
    ui->lblStatus->setStyleSheet("color: green;");
    ui->btnCreateUser->setVisible(false); // Ocultar por seguridad

    // 4. Ocultar datos de la tabla
    if(m_model) {
        m_model->setFilter("1=0");
    }
}

// ---------------------------------------------------------
// AGREGAR DISPOSITIVO
// ---------------------------------------------------------
void MainWindow::on_btnAddDevice_clicked()
{
    DeviceDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {

        Device *newDevice = dialog.getDeviceInfo();

        // Usar el ID del usuario real logueado
        int currentUserId = m_user.getId();
        if (currentUserId <= 0) currentUserId = 1; // Fallback

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

// ---------------------------------------------------------
// BORRAR DISPOSITIVO
// ---------------------------------------------------------
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

// ---------------------------------------------------------
// EDITAR DISPOSITIVO
// ---------------------------------------------------------
void MainWindow::on_btnEditDevice_clicked()
{
    QModelIndexList selectedRows = ui->tableDevices->selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QMessageBox::warning(this, "Editar", "Selecciona el dispositivo a editar.");
        return;
    }

    int row = selectedRows.at(0).row();

    // Obtener datos usando fieldIndex para mayor seguridad
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

        // Preservar ID original y UserID
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
// BUSCADOR
// ---------------------------------------------------------
void MainWindow::on_txtSearch_textChanged(const QString &arg1)
{
    if (!m_model) return;

    if (arg1.isEmpty()) {
        m_model->setFilter("");
    }
    else {
        QString filter = QString("name LIKE '%%1%' OR ip_address LIKE '%%1%'").arg(arg1);
        m_model->setFilter(filter);
    }
    m_model->select();
}

// ---------------------------------------------------------
// EXPORTAR A CSV
// ---------------------------------------------------------
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

    // Escribir cabeceras
    QStringList headers;
    headers << "ID" << "Usuario_ID" << "Nombre" << "Tipo" << "IP" << "Calibracion";
    out << headers.join(";") << "\n";

    // Escribir datos
    for (int row = 0; row < m_model->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < m_model->columnCount(); ++col) {
            QString data = m_model->data(m_model->index(row, col)).toString();
            data.replace(";", ",");
            rowData << data;
        }
        out << rowData.join(";") << "\n";
    }

    file.close();
    QMessageBox::information(this, "Éxito", "Datos exportados correctamente a:\n" + fileName);
}

// ---------------------------------------------------------
// CREAR USUARIO (Solo Admin)
// ---------------------------------------------------------
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
