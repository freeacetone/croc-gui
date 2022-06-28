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

#include "version.h"

#include <QRegularExpression>
#include <QDebug>

Version::Version(const QString &rawVersion)
{
    set(rawVersion);
}

void Version::set(const QString &rawVersion)
{
    m_raw = rawVersion;

    QString preparedStr {rawVersion};
    preparedStr.remove(QRegularExpression("[^0-9\\\\.]"));
    preparedStr.remove(QRegularExpression("^\\\\.*"));
    while (preparedStr.contains(".."))
    {
        preparedStr.replace("..", ".");
    }

    if (preparedStr.isEmpty())
    {
        return;
    }

    int dotPos = preparedStr.indexOf(".");
    if (dotPos == -1) // only major
    {
        m_major = preparedStr.toUInt();
        return;
    }
    m_major = preparedStr.mid(0, dotPos).toUInt();
    preparedStr.remove(0, dotPos+1);

    dotPos = preparedStr.indexOf(".");
    if (dotPos == -1) // without revision
    {
        m_minor = preparedStr.toUInt();
        return;
    }
    m_minor = preparedStr.mid(0, dotPos).toUInt();
    preparedStr.remove(0, dotPos+1);

    dotPos = preparedStr.indexOf(".");
    if (dotPos != -1) // looks like some shit existed after revision number
    {
        preparedStr.remove(dotPos, preparedStr.size()-dotPos);
        m_revision = preparedStr.toUInt();
        return;
    }
    m_revision = preparedStr.toUInt();
}

QString Version::string() const
{
    return QString::number(major()) + "." + QString::number(minor()) + "." + QString::number(revision());
}

QString Version::raw() const
{
    return m_raw;
}

bool Version::operator==(const Version &another)
{
    return major()    == another.major() and
           minor()    == another.minor() and
           revision() == another.revision();
}

bool Version::operator<(const Version &another)
{
    return major()    < another.major() or
           minor()    < another.minor() or
           revision() < another.revision();
}

bool Version::operator>(const Version &another)
{
    return major()    > another.major() or
           minor()    > another.minor() or
           revision() > another.revision();
}
