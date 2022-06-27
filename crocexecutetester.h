#ifndef CROCEXECUTETESTER_H
#define CROCEXECUTETESTER_H

#include <QThread>

class CrocExecuteTester : public QThread
{
    Q_OBJECT
public:
    explicit CrocExecuteTester(QObject *parent = nullptr) : QThread(parent) {}
    void set (const QString& command);
    bool result() const { return m_status; }
    QString version() const { return m_version; }

private:
    QString m_command;
    QString m_version;
    bool m_status = false;

    // QThread interface
protected:
    void run();
};

#endif // CROCEXECUTETESTER_H
