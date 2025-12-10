#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

#include <QDialog>
#include "device.h"

namespace Ui {
class DeviceDialog;
}

/**
 * @brief Diálogo modal para la creación y edición de dispositivos.
 *
 * Esta clase gestiona la interfaz gráfica donde el usuario ingresa los datos
 * (Nombre, Tipo, IP, Calibración). Incluye validaciones de entrada (como Regex para IP)
 * y permite rellenar los campos automáticamente si se está en modo edición.
 */
class DeviceDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la clase DeviceDialog.
     * Configura la interfaz y aplica validadores (ej. QRegularExpressionValidator para IP).
     * @param parent Widget padre (opcional).
     */
    explicit DeviceDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor de la clase.
     * Libera la memoria asignada a la interfaz de usuario (UI).
     */
    ~DeviceDialog();

    /**
     * @brief Genera un objeto Device con la información ingresada en el formulario.
     *
     * @note Esta función asigna memoria dinámica (new Device). Es responsabilidad
     * del llamador (MainWindow) gestionar la destrucción (delete) de este objeto
     * para evitar fugas de memoria.
     *
     * @return Puntero a un nuevo objeto Device configurado.
     */
    Device* getDeviceInfo();

    /**
     * @brief Carga los datos de un dispositivo existente en los campos del formulario.
     * Se utiliza cuando el usuario desea editar un dispositivo ya creado.
     * @param device Puntero al dispositivo cuyos datos se van a visualizar.
     */
    void setDeviceData(Device *device);

private slots:
    /**
     * @brief Slot ejecutado al presionar el botón "Guardar".
     * Realiza validaciones finales (campos vacíos) y acepta el diálogo si todo es correcto.
     */
    void on_btnSave_clicked();

    /**
     * @brief Slot ejecutado al presionar el botón "Cancelar".
     * Cierra el diálogo y descarta los cambios (reject).
     */
    void on_btnCancel_clicked();

private:
    /**
     * @brief Puntero a la clase de interfaz generada por Qt Designer.
     */
    Ui::DeviceDialog *ui;

    /**
     * @brief Puntero temporal utilizado para almacenar o transferir el dispositivo.
     */
    Device *m_device;
};

#endif // DEVICEDIALOG_H
