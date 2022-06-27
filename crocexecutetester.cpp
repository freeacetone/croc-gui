#include "crocexecutetester.h"

#include <QProcess>
#include <QRegularExpression>

void CrocExecuteTester::set(const QString &command)
{
    m_command = command;
}

void CrocExecuteTester::run()
{
    QProcess proc;
    proc.setProgram(m_command);
    proc.setNativeArguments("--version");
    proc.start();
    proc.waitForFinished();
    QString out = proc.readAllStandardOutput();
    if (out.startsWith("croc version"))
    {
        m_status = true;
        // croc version v9.5.6-b7e4a73\n
        // xxxxxxxxxxxxxx     xxxxxxxxxx
        out.remove(QRegularExpression("^croc version v"));
        out.remove(QRegularExpression("-.*$"));
        out.remove(QRegularExpression("[^0-9\\.]"));
        m_version = out;
    }

    QThread::quit();
}
