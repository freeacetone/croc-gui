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

#ifndef VERSION_H
#define VERSION_H

#include <QString>

class Version
{
public:
    Version() {}
    Version(const QString& rawVersion);
    void set(const QString& rawVersion);
    uint major() const { return m_major; }
    uint minor() const { return m_minor; }
    uint revision() const { return m_revision; }

    QString string() const;
    QString raw() const;

    bool operator==(const Version& another);
    bool operator<(const Version& another);
    bool operator>(const Version& another);

private:
    QString m_raw;

    uint m_major = 0;
    uint m_minor = 0;
    uint m_revision = 0;
};

#endif // VERSION_H
