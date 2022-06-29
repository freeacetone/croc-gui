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

#ifndef RESULTPAIR_H
#define RESULTPAIR_H

#include <QString>

class ResultPair
{
public:
    ResultPair (bool status, QString string = "") : m_str(string), m_bool(status) {}
    bool status() const { return m_bool; }
    QString string() const { return m_str; }

private:
    QString m_str;
    bool m_bool;
};

#endif // RESULTPAIR_H
