#include "croccliprocess.h"
#include "database.h"

CrocCliProcess::CrocCliProcess(const QStringList &args, QObject *parent) : QObject(parent)
{
    setArgs(args);
}

CrocCliProcess::~CrocCliProcess()
{
    if (m_process)
    {
        m_process->terminate();
        m_process->deleteLater();
    }
}

void CrocCliProcess::setArgs(const QStringList &args)
{
    m_arguments = args;
}

void CrocCliProcess::start()
{
    m_process = new QProcess;
    m_process->setProgram(Database::unescape( Database().getAppSetting(db::CROC_EXECUTE_COMMAND_KEY) ));
    m_process->setArguments(m_arguments);

    connect (
        m_process, &QProcess::readyRead, this,
        [&]() {
            emit stdOutChanged( m_process->readAll() );
        }
    );

    connect (m_process, SIGNAL(finished(int)), this, SIGNAL(finished(int)));
    m_process->start();
}
