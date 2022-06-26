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
