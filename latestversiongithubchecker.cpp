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

#include "latestversiongithubchecker.h"

#include <QSslSocket>
#include <QRegularExpression>

void LatestVersionGitHubChecker::setRepo(const QString &repo)
{
    m_repo = repo;
}

void LatestVersionGitHubChecker::run()
{
    QSslSocket socket;
    QString response;

    if (m_repo.isEmpty())
    {
        m_errorString = "Repository is undefined";
        goto END;
    }

    socket.connectToHostEncrypted("github.com", 443);
    if (not socket.waitForConnected() or not socket.waitForEncrypted())
    {
        m_errorString = "Can't check";
        goto END;
    }

    m_repo.remove(QRegularExpression("^/"));
    m_repo.remove(QRegularExpression("/$"));
    socket.write("GET /"+ m_repo.toUtf8() +"/releases/latest HTTP/1.0\r\n"
                 "Host: github.com\r\n"
                 "\r\n");

    if (not socket.waitForReadyRead())
    {
        m_errorString = "Response timeout";
        goto END;
    }

    response = socket.readAll();
    response.remove(QRegularExpression("^.*Location: ", QRegularExpression::DotMatchesEverythingOption));
    response.remove(QRegularExpression("\r\n.*$", QRegularExpression::DotMatchesEverythingOption));
    response.remove(QRegularExpression("^.*/"));
    if (response.isEmpty())
    {
        m_errorString = "Unknown";
        goto END;
    }
    if (response == "releases")
    {
        m_errorString = "No releases";
        goto END;
    }

    m_status = true;
    m_version.set(response);

END:
    QThread::quit();
}
