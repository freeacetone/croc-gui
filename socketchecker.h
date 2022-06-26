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

#ifndef SOCKETCHECKER_H
#define SOCKETCHECKER_H

#include <QObject>
#include <QHostAddress>
#include <QThread>

class SocketChecker : public QThread
{
    Q_OBJECT
public:
    explicit SocketChecker (QObject *parent = nullptr) : QThread(parent) {}
    void set (QHostAddress address, quint16 port);
    bool result() const { return m_result; }

private:
    QHostAddress m_address;
    quint16 m_port = 0;
    bool m_result = false;

    // QThread interface
protected:
    void run();
};

#endif // SOCKETCHECKER_H
