#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>
#include "databasemanager.h"

// ---------------------------------------------------------
// CONSTRUCTOR Y DESTRUCTOR
// ---------------------------------------------------------

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

// ---------------------------------------------------------
// BOTONES DE ACCIÓN
// ---------------------------------------------------------

void RegisterDialog::on_btnSave_clicked()
{
    QString user = ui->txtUser->text();
    QString pass = ui->txtPass->text();
    QString role = ui->comboRole->currentText();

    if (user.isEmpty() || pass.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Todos los campos son obligatorios.");
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO users (username, password, role) VALUES (:u, :p, :r)");
    query.bindValue(":u", user);
    query.bindValue(":p", pass);
    query.bindValue(":r", role);

    if (query.exec()) {
        QMessageBox::information(this, "Éxito", "Usuario creado correctamente.");
        accept();
    } else {
        QMessageBox::critical(this, "Error", "No se pudo crear el usuario.\n" + query.lastError().text());
    }
}

void RegisterDialog::on_btnCancel_clicked()
{
    reject();
}
