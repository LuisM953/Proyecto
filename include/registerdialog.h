#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include <QSqlDatabase>

namespace Ui {
class RegisterDialog;
}

/**
 * @brief Diálogo modal para el registro de nuevos usuarios en el sistema.
 *
 * Esta ventana permite ingresar un nombre de usuario, contraseña y asignar un rol (admin/user).
 * Realiza la inserción directa en la tabla 'users' de la base de datos tras validar
 * que los campos no estén vacíos.
 */
class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor de la clase RegisterDialog.
     * Configura la interfaz de usuario.
     * @param parent Widget padre (opcional).
     */
    explicit RegisterDialog(QWidget *parent = nullptr);

    /**
     * @brief Destructor de la clase.
     * Libera los recursos de la interfaz.
     */
    ~RegisterDialog();

private slots:
    /**
     * @brief Slot ejecutado al presionar el botón "Guardar".
     *
     * 1. Valida que usuario y contraseña no estén vacíos.
     * 2. Ejecuta una consulta SQL INSERT en la tabla 'users'.
     * 3. Muestra mensajes de éxito o error y cierra el diálogo si tuvo éxito.
     */
    void on_btnSave_clicked();

    /**
     * @brief Slot ejecutado al presionar el botón "Cancelar".
     * Cierra el diálogo descartando la operación (reject).
     */
    void on_btnCancel_clicked();

private:
    /**
     * @brief Puntero a la interfaz gráfica generada por Qt Designer.
     */
    Ui::RegisterDialog *ui;
};

#endif // REGISTERDIALOG_H
