#include "licensewindow.h"
#include "ui_licensewindow.h"

#include <QFile>
#include <QIcon>

LicenseWindow::LicenseWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/files/gnu.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("GPLv3 full text");

    QFile txt (":/LICENSE");
    if (not txt.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error("Can not read GPLv3 text");
    }
    ui->textEdit->setText(txt.readAll());
    txt.close();
}

LicenseWindow::~LicenseWindow()
{
    delete ui;
}
