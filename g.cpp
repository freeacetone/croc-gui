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

#include "g.h"

#include <random>

namespace g {

QString randomStr (int entropy, int sizeOfLine)
{
    constexpr char randomtable[60] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                                      'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                                      'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
                                      'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D',
                                      'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
                                      'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X'};

    QString random_value;
    if(entropy <= 0 || entropy > 59)
    {
        entropy = 59;
    }

    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, entropy);

    while(random_value.size() < sizeOfLine)
    {
        random_value += randomtable[dist(rd)];
    }

    return random_value;
}

} // namespace
