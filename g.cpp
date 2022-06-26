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
