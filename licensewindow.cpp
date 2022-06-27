/*
    Croc GUI. Additional tool for croc (https://github.com/schollz/croc):
    "Easily and securely send things from one computer to another"
    Copyright (C) 2022, Croc GUI team

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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
