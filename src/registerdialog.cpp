#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include "databasemanager.h" // Incluimos esto para acceder a la conexión

RegisterDialog::RegisterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    setWindowTitle("Registrar Nuevo Usuario");
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_btnSave_clicked()
{
    QString user = ui->txtUser->text();
    QString pass = ui->txtPass->text();
    QString role = ui->comboRole->currentText();

    // 1. Validaciones básicas
    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Todos los campos son obligatorios.");
        return;
    }

    // 2. Preparar la consulta
    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, role) VALUES (:u, :p, :r)");
    query.bindValue(":u", user);
    query.bindValue(":p", pass);
    query.bindValue(":r", role);

    // 3. Ejecutar
    if (query.exec()) {
        QMessageBox::information(this, "Éxito", "Usuario creado correctamente.");
        accept(); // Cerrar ventana
    } else {
        QMessageBox::critical(this, "Error", "No se pudo crear el usuario.\n" + query.lastError().text());
    }
}

void RegisterDialog::on_btnCancel_clicked()
{
    reject();
}
