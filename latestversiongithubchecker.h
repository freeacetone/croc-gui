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

#ifndef LATESTVERSIONGITHUBCHECKER_H
#define LATESTVERSIONGITHUBCHECKER_H

#include "version.h"

#include <QThread>

class LatestVersionGitHubChecker : public QThread
{
    Q_OBJECT
public:
    explicit LatestVersionGitHubChecker(QObject *parent = nullptr) : QThread(parent) {}
    void setRepo(const QString& repo); // e.g. user/repo

    Version version() const { return m_version; }
    bool status() const { return m_status; }
    QString errorString() const { return m_errorString; }

private:
    Version m_version;
    QString m_repo;
    QString m_errorString;
    bool m_status = false;

    // QThread interface
protected:
    void run();
};

#endif // LATESTVERSIONGITHUBCHECKER_H
