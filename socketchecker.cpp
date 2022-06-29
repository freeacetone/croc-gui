/*
    Croc GUI. Additional tool for croc (https://github.com/schollz/croc):
    "Easily and securely send things from one computer to another"
    Copyright (C) 2022, acetone

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

#include "socketchecker.h"

#include <QTcpSocket>

void SocketChecker::set(QHostAddress address, quint16 port)
{
    m_address = address;
    m_port = port;
}

void SocketChecker::run()
{
    QTcpSocket socket;
    socket.connectToHost(m_address, m_port);
    if (socket.waitForConnected(5000))
    {
        m_result = true;
        socket.disconnectFromHost();
    }
    else
    {
        m_result = false;
    }

    QThread::quit();
}
