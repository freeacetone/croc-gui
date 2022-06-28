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

#ifndef INFORMATIONWINDOW_H
#define INFORMATIONWINDOW_H

#include <QDialog>

namespace Ui {
class InformationWindow;
}

class InformationWindow : public QDialog
{
    Q_OBJECT

public:
    explicit InformationWindow(QWidget *parent = nullptr);
    ~InformationWindow();

private:
    Ui::InformationWindow *ui;
};

#endif // INFORMATIONWINDOW_H
