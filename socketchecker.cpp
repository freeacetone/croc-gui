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
