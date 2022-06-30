#ifndef CROCCLIPROCESS_H
#define CROCCLIPROCESS_H

#include <QObject>
#include <QProcess>

class CrocCliProcess : public QObject
{
    Q_OBJECT
public:
    explicit CrocCliProcess (QObject *parent = nullptr) : QObject(parent) {}
    explicit CrocCliProcess (const QStringList& args, QObject *parent = nullptr);
    ~CrocCliProcess();

    void setArgs(const QStringList& args);

private:
    QProcess* m_process = nullptr;
    QStringList m_arguments;

public slots:
    void start();

signals:
    void stdOutChanged (QString stdoutOutput);
    void errorOccured (QString error);
    void finished (int exitCode);
};

#endif // CROCCLIPROCESS_H
