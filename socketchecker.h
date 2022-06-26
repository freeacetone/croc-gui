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
