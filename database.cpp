#include "database.h"
#include "g.h"

#include <QApplication>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QMessageBox>
#include <QDateTime>

QMutex Database::m_staticMutex;
bool Database::m_staticInited = false;
const QString DB_CONN_NAME = g::randomStr();

Database::Database()
{
    m_staticMutex.lock();

    m_db = QSqlDatabase::addDatabase("QSQLITE", DB_CONN_NAME);
    m_db.setDatabaseName(db::DATABASE_PATH);

    if (not m_db.open())
    {
       throw std::runtime_error("Connection with database failed");
    }

    if (not m_staticInited)
    {
        init();
        m_staticInited = true;
    }
}

Database::~Database()
{
    m_db.close();
    m_db = QSqlDatabase();
    QSqlDatabase::removeDatabase(DB_CONN_NAME);
    m_staticMutex.unlock();
}

QSqlDatabase *Database::raw()
{
    return &m_db;
}

void Database::appendToHistory(db::Operation operation, const QString &filename, const QString& fullpath)
{
    QSqlQuery query(m_db);

    int limit = getAppSetting(db::HISTORY_LIMIT_KEY).toInt();

    if (not query.exec("SELECT COUNT(*) FROM " + db::HISTORY_TABLE))
    {
        qInfo().noquote() << __FUNCTION__ << "failed:" << query.lastError().text();
        return;
    }

    if (not query.next())
    {
        qInfo().noquote() << __FUNCTION__ << "failed: can not check total count";
        return;
    }

    bool compressed = false;

    int count = query.value(0).toInt();

    while ( (limit>0 ? count+1 : count) > limit)
    {
        compressed = true;
        if (not query.exec("DELETE FROM " + db::HISTORY_TABLE + " WHERE rowid = (SELECT MIN(rowid) FROM "+db::HISTORY_TABLE+")"))
        {
            qInfo().noquote() << __FUNCTION__ << "delete failed:"  << query.lastError().text();
            break;
        }

        query.exec("SELECT COUNT(*) FROM " + db::HISTORY_TABLE);
        if (query.next())
        {
            count = query.value(0).toInt();
        }
        else
        {
            break;
        }
    }

    if (compressed)
    {
        query.exec("VACUUM");
    }

    if (limit == 0) return;

    if (not query.exec("INSERT INTO " + db::HISTORY_TABLE + " (Action_type, Operation_date, File_name, File_path) "
                       "VALUES ('" +
                        (operation == db::Operation::Sent ? "Sent" : "Received") + "', '" +
                        QDateTime::currentDateTime().toString(Qt::DateFormat::SystemLocaleShortDate) + "', '" +
                        escape(filename) + "', '" +
                        escape(fullpath) +
                       "')"))
    {
        qInfo().noquote() << __FUNCTION__ << "insert failed:" << query.lastError().text();
    }
}

ResultPair Database::setAppSetting(const QString &key, const QString &value)
{
    QSqlQuery query(m_db);

    bool status = false;

    status = query.exec("INSERT OR REPLACE INTO "+db::SETTINGS_TABLE+"(param, val) VALUES ('"+key+"', '"+value+"')");

    if (not status)
    {
        qInfo().noquote() << __FUNCTION__ << "db query failed:" << query.lastError().text();
    }

    return status ? ResultPair(true) : ResultPair(false, query.lastError().text());
}

QString Database::getAppSetting(const QString &key)
{
    QSqlQuery query(m_db);

    if (not query.exec("SELECT * FROM "+db::SETTINGS_TABLE+" WHERE param = '" + key + "'"))
    {
        qInfo().noquote() << __FUNCTION__ << "db query failed:" << query.lastError().text();
        return QString();
    }

    if (query.next())
    {
        return query.value(1).toString();
    }

    return QString();
}

QString Database::escape(const QString &text)
{
    QString result {text};
    result.replace ("`", "ESCAPED_HATCH0");
    result.replace ("'", "ESCAPED_HATCH1");
    result.replace (";", "ESCAPED_COMMA_DOT");
    return result;
}

QString Database::unescape(const QString &text)
{
    QString result {text};
    result.replace ("ESCAPED_HATCH0",    "`");
    result.replace ("ESCAPED_HATCH1",    "'");
    result.replace ("ESCAPED_COMMA_DOT", ";");
    return result;
}

void Database::init()
{
    QSqlQuery query(m_db);

    bool appSettings = query.exec ("CREATE TABLE "+db::SETTINGS_TABLE+" (param VARCHAR PRIMARY KEY, val VARCHAR)");
    if (appSettings)
    {
        setAppSetting(db::DATABASE_VERSION_KEY, QString::number(db::DATABASE_VER));
        setAppSetting(db::HISTORY_LIMIT_KEY, QString::number(db::HISTORY_SIZE_DEFAULT));
        // https://github.com/schollz/croc#change-encryption-curve
        setAppSetting(db::ENCRYPTION_CURVE_KEY, "P-256");
        // https://github.com/schollz/croc#change-hash-algorithm
        setAppSetting(db::HASH_ALGORITHM_KEY, "xxhash");
    }
    else
    {
        QString version = getAppSetting(db::DATABASE_VERSION_KEY);
        if (version != QString::number(db::DATABASE_VER))
        {
            m_db.close();
            if (QFile::remove(db::DATABASE_PATH))
            {
                if (m_db.open())
                {
                    init();
                    return;
                }
            }

            QMessageBox* msg =  new QMessageBox();
            msg->setText("Database version required is " + QString::number(db::DATABASE_VER) + ", current is " + version + ". "
                         "Automatic remove of existed database file failed so you should delete it manually (" + db::DATABASE_PATH + ") "
                         "and start application again.");
            QObject::connect(msg, &QMessageBox::finished, [](){
               QApplication::exit(1);
            });
            msg->show();
        }
    }

    bool history = query.exec (
                      "CREATE TABLE "+db::HISTORY_TABLE+
                      "(Action_type VARCHAR, "
                       "Operation_date VARCHAR, "
                       "File_name VARCHAR, "
                       "File_path TEXT)"
                   );

    if (appSettings and history)
    {
        qInfo().noquote() << "Database created";
    }
}
