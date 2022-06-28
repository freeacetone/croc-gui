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

#include "crocexecutetester.h"

#include <QProcess>
#include <QDebug>
#include <QRegularExpression>

void CrocExecuteTester::set(const QString &command)
{
    m_command = command;
}

void CrocExecuteTester::run()
{
    QProcess proc;
    proc.setProgram(m_command);
    proc.setArguments(QStringList() << "--version");
    proc.start();
    if (proc.waitForFinished())
    {
        QString out = proc.readAllStandardOutput();
        if (out.startsWith("croc version"))
        {
            m_status = true;
            // croc version v9.5.6-b7e4a73\n
            // xxxxxxxxxxxxxx     xxxxxxxxxx
            out.remove(QRegularExpression("^croc version v"));
            out.remove(QRegularExpression("-.*$"));
            out.remove(QRegularExpression("[^0-9\\.]"));
            m_version.set(out);
        }
    }

    QThread::quit();
}
