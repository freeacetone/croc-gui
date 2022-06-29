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

const QString TARGET_SAVE_DIRECTORY = "target_save_dir";
const QString SENDER_IP = "sender_ip";
const QString ONLY_LOCAL_NETWORK = "only_local_network";
const QString LAST_OPENED_TAB_KEY = "last_opened_tab";
const QString HISTORY_LIMIT_KEY = "history_limit";
const QString DATABASE_VERSION_KEY = "database_ver";
const QString CROC_EXECUTE_COMMAND_KEY = "croc_execute";
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
    void          appendToHistory(db::Operation, const QString& filename, const QString& fullpath);
    void          removeFromHistory(const quint64 rowid);
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
