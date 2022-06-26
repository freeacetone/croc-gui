#ifndef DATABASE_H
#define DATABASE_H

#include "resultpair.h"

#include <QDir>
#include <QMutex>
#include <QSqlDatabase>

namespace db {

constexpr const uint8_t DATABASE_VER = 1;
constexpr const int HISTORY_SIZE_DEFAULT = 40;
const QString DATABASE_PATH = QDir::homePath()+"/crocgui.db";

const QString SETTINGS_TABLE = "app_settings";
const QString HISTORY_TABLE = "history_log";

const QString HISTORY_LIMIT_KEY = "history_limit";
const QString DATABASE_VERSION_KEY = "database_ver";
const QString PROXY_ADDRESS_KEY = "proxy_address";
const QString CUSTOM_RELAY_ADDRESS_KEY = "custom_relay";
const QString CUSTOM_CODE_PHRASE_KEY = "custom_code_phrase";
const QString ENCRYPTION_CURVE_KEY = "encryption_curve";
const QString HASH_ALGORITHM_KEY = "hash_algorithm";
const QString OVERWRAITING_WITHOUT_PROMT_KEY = "overwriting_auto_allow";

enum class Operation {
    Received,
    Sent
};

} // namespace db

class Database
{
public:
    Database();
    ~Database();

    QSqlDatabase* raw();
    void          appendToHistory(db::Operation, const QString& filename);
    ResultPair    setAppSetting(const QString &key, const QString &value);
    QString       getAppSetting(const QString &key);

    static QString escape(const QString& text);
    static QString unescape(const QString& text);

private:
    void init();

    QSqlDatabase m_db;
    static QMutex m_staticMutex;
    static bool m_staticInited;
};

#endif // DATABASE_H
