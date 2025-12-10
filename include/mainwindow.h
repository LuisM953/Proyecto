#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlTableModel>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QDir>
#include "databasemanager.h"
#include "user.h"
#include "registerdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/**
 * @brief Clase principal de la aplicación que gestiona la interfaz gráfica de usuario (GUI).
 *
 * Esta clase orquesta el flujo completo de la aplicación:
 * - Gestiona el StackedWidget para cambiar entre Login y Dashboard.
 * - Maneja la sesión del usuario mediante la clase User.
 * - Coordina las operaciones CRUD visuales sobre la tabla de dispositivos.
 * - Aplica estilos y configuraciones visuales.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la ventana principal.
     * Inicializa la interfaz (UI), aplica los estilos (Dark Mode) y abre la conexión a la BD.
     * @param parent Widget padre (generalmente nullptr para la ventana raíz).
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor de la clase.
     * Libera la memoria de la interfaz y del modelo de datos si fue creado.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot ejecutado al pulsar el botón "Iniciar Sesión".
     * Valida las credenciales ingresadas, registra el log y cambia a la vista principal.
     */
    void on_btnLogin_clicked();

    /**
     * @brief Slot ejecutado al pulsar el botón "Cerrar Sesión".
     * Limpia los datos de la sesión actual (`m_user`) y retorna a la vista de Login.
     */
    void on_btnLogout_clicked();

    /**
     * @brief Slot para abrir el diálogo de creación de un nuevo dispositivo.
     */
    void on_btnAddDevice_clicked();

    /**
     * @brief Slot para eliminar el dispositivo seleccionado en la tabla.
     * Solicita confirmación al usuario antes de proceder con el borrado en la BD.
     */
    void on_btnDeleteDevice_clicked();

    /**
     * @brief Slot para editar el dispositivo seleccionado.
     * Abre el DeviceDialog precargando los datos de la fila seleccionada.
     */
    void on_btnEditDevice_clicked();

    /**
     * @brief Slot ejecutado cuando el texto de la barra de búsqueda cambia.
     * Aplica un filtro SQL (WHERE) al modelo para buscar por nombre o IP en tiempo real.
     * @param arg1 El texto actual introducido por el usuario.
     */
    void on_txtSearch_textChanged(const QString &arg1);

    /**
     * @brief Slot para exportar los datos visibles de la tabla a un archivo CSV.
     * Abre un cuadro de diálogo para seleccionar la ubicación de guardado.
     */
    void on_btnExport_clicked();

    /**
     * @brief Slot para abrir el diálogo de registro de nuevos usuarios.
     * @note Este botón solo es visible si el usuario logueado tiene rol de Administrador.
     */
    void on_btnCreateUser_clicked();

private:
    /**
     * @brief Puntero a la interfaz gráfica generada por Qt Designer.
     */
    Ui::MainWindow *ui;

    /**
     * @brief Instancia encargada de gestionar la conexión persistente con la base de datos.
     */
    DatabaseManager m_dbManager;

    /**
     * @brief Objeto que mantiene el estado de la sesión del usuario actual (ID, Rol, Nombre).
     */
    User m_user;

    /**
     * @brief Modelo de datos SQL que enlaza la tabla 'devices' de la BD con la vista visual (QTableView).
     * Permite visualizar, ordenar y filtrar los datos sin escribir SQL manual en la vista.
     */
    QSqlTableModel *m_model;

    /**
     * @brief Configura las propiedades de la tabla de dispositivos.
     * Establece el modelo, define los encabezados amigables, oculta columnas internas (ID)
     * y ajusta el modo de redimensionamiento de columnas.
     */
    void setupDevicesTable();

    /**
     * @brief Aplica una hoja de estilos (QSS) global a la aplicación.
     * Define colores, bordes y fuentes para lograr el tema oscuro (Dark Mode).
     */
    void applyStyles();
};
#endif // MAINWINDOW_H
