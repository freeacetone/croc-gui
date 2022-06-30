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

#include "widget.h"
#include "ui_widget.h"
#include "database.h"
#include "licensewindow.h"
#include "crocexecutetester.h"
#include "informationwindow.h"

#include <QDebug>
#include <QThread>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlQueryModel>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QMenu>
#include <QClipboard>
#include <QFileDialog>
#include <QProcess>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Global UI

    setWindowTitle("Croc GUI");
    setWindowIcon(QIcon(":/files/crocodile.png"));

    connect (
        ui->footer_RepoButton, &QPushButton::clicked,
        [&]() {
            QDesktopServices::openUrl(QUrl("https://github.com/freeacetone/croc-gui"));
        }
    );
    connect (
        ui->footer_LicenseButton, &QPushButton::clicked,
        [&]() {
            (new LicenseWindow(this))->show();
        }
    );
    connect (
        ui->footer_informationButton, &QPushButton::clicked,
        [&]() {
            (new InformationWindow(this))->show();
        }
    );

    QString lastActiveTab = Database().getAppSetting(db::LAST_OPENED_TAB_KEY);
    if (lastActiveTab == "Send")
    {
        ui->tabWidget->setCurrentIndex(0);
        initSendTab();
    }
    else if (lastActiveTab == "Receive")
    {
        ui->tabWidget->setCurrentIndex(1);
        initReceiveTab();
    }

    connect (
        ui->tabWidget, &QTabWidget::tabBarClicked,
        [&]( int index ) {
            if (index == 0)
            {
                Database().setAppSetting(db::LAST_OPENED_TAB_KEY, "Send");
                initSendTab();
            }
            else if (index == 1)
            {
                Database().setAppSetting(db::LAST_OPENED_TAB_KEY, "Receive");
                initReceiveTab();
            }
            else if (index == 2)
            {
                initHistoryTab();
            }
            else if (index == 3)
            {
                initSettingsTab();
            }
        }
    );

    // Send

    connect (ui->send_filepathLineEdit, &QLineEdit::textEdited, this, &Widget::checkFilepathToSend);
    connect (ui->send_filepathLineEdit, &QLineEdit::textChanged, this, &Widget::checkFilepathToSend);
    connect (
        ui->send_selectFileButton, &QPushButton::clicked,
        [&]() {
            QList<QUrl> pathsUrls = QFileDialog::getOpenFileUrls();
            if (pathsUrls.isEmpty()) return;

            QList<QString> pathList;
            for (const auto& url: pathsUrls)
            {
                QString path = url.toString();
#ifdef WIN32
                path.remove(QRegularExpression("^file:///"));
#endif
                pathList << path;
            }

            QString pathsString;
            for (auto it = pathList.begin(); it != pathList.end(); ++it)
            {
                if (not pathsString.isEmpty())
                {
                    pathsString += " ";
                }
                pathsString += '\"' + *it + '\"';
            }

            ui->send_filepathLineEdit->setText(pathsString);
        }
    );
    connect (
        ui->send_selectFolderButton, &QPushButton::clicked,
        [&]() {
            QFileDialog* dialog = new QFileDialog(this);
            dialog->setFileMode(QFileDialog::Directory);
            if (dialog->exec())
            {
                QString path = dialog->selectedFiles().first();
                if (path.isEmpty()) return;
                path.remove(QRegularExpression("^file:///"));
                ui->send_filepathLineEdit->setText('\"'+path+'\"');
            }
            dialog->deleteLater();
        }
    );
    connect (
        ui->send_codePhraseField, &QLineEdit::textChanged,
        [&]() {
            QString phrase = ui->send_codePhraseField->text();
            ui->send_codePhraseField->setText(phrase.remove(QRegularExpression("[^a-z0-9\\-]")));
        }
    );
    connect (
        ui->send_codePhraseRememberButton, &QPushButton::clicked,
        [&]() {
            QString phrase = ui->send_codePhraseField->text();
            Database().setAppSetting(db::CUSTOM_CODE_PHRASE_KEY, phrase);
            ui->send_codePhraseRememberButton->setText("Complete");
            QTimer* timer = new QTimer;
            timer->setSingleShot(true);
            timer->setInterval(2000);
            connect (
                timer, &QTimer::timeout,
                [=]() {
                    ui->send_codePhraseRememberButton->setText("Remember");
                    timer->deleteLater();
                }
            );
            timer->start();
        }
    );

    // Receive

    connect (
        ui->receive_codeField, &QLineEdit::textEdited,
        [&]() {
            ui->receive_receiveButton->setDisabled( ui->receive_codeField->text().isEmpty() );
        }
    );

    connect (
        ui->receive_TargetDirSelectButton, &QPushButton::clicked,
        [&]() {
            QFileDialog* dialog = new QFileDialog(this);
            dialog->setFileMode(QFileDialog::Directory);
            if (dialog->exec())
            {
                QString path = dialog->selectedFiles().first();
                if (path.isEmpty()) return;
                path.remove(QRegularExpression("^file:///"));
                ui->receive_TargetDirField->setText('\"'+path+'\"');
            }
            dialog->deleteLater();
        }
    );

    connect (
        ui->receive_senderIpTestButton, &QPushButton::clicked,
        [&]() {
            QString address = ui->receive_senderIpField->text();
            if (address.isEmpty()) return;

            if (address.startsWith("[")) // ipv6
            {
                address.remove(QRegularExpression("\\].*$"));
                address.remove(QRegularExpression("[^0-9a-f:]"));
            }
            else
            {
                address.remove(QRegularExpression(":.*$"));
                address.remove(QRegularExpression("[^0-9\\.]"));
            }

            ui->receive_senderIpField->setText(address);

            SocketChecker* checker = new SocketChecker;
            checker->set(QHostAddress(address), 9009);
            connect (
                checker, &SocketChecker::finished, this,
                [=]() {
                    ui->receive_senderIpTestButton->setText(checker->result() ? "Reachable" : "Refused");
                    QTimer* timer = new QTimer;
                    timer->setSingleShot(true);
                    timer->setInterval(2000);
                    connect (
                        timer, &QTimer::timeout,
                        [=]() {
                            ui->receive_senderIpTestButton->setText("Test");
                            timer->deleteLater();
                        }
                    );
                    timer->start();
                    checker->deleteLater();
                },
                Qt::QueuedConnection
            );
            checker->start();
        }
    );

    connect (
        ui->receive_rememberButton, &QPushButton::clicked,
        [&]() {
            Database db;
            db.setAppSetting(db::TARGET_SAVE_DIRECTORY, Database::escape( ui->receive_TargetDirField->text() ));
            db.setAppSetting(db::SENDER_IP, Database::escape( ui->receive_senderIpField->text() ));
            db.setAppSetting(db::ONLY_LOCAL_NETWORK, ui->receive_onlyLocalEnableRadioButton->isChecked() ? "true" : "false");

            ui->receive_rememberButton->setText("Complete");
            QTimer* timer = new QTimer;
            timer->setSingleShot(true);
            timer->setInterval(2000);
            connect (
                timer, &QTimer::timeout,
                [=]() {
                    ui->receive_rememberButton->setText("Remember");
                    timer->deleteLater();
                }
            );
            timer->start();
        }
    );

    // History

    ui->history_tableView->verticalHeader()->setSectionsClickable(false);
    ui->history_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect (ui->history_tableView, &QTableView::customContextMenuRequested, this, &Widget::historyMenuRequest);

    // Settings
#ifdef WIN32
    ui->settings_crocCallField->setPlaceholderText("croc.exe or path/to/croc.exe");
#else
    ui->settings_crocCallField->setPlaceholderText("croc or /path/to/croc");
#endif
    ui->settings_curveComboBox->addItems (QStringList() << "P-256" << "P-348" << "P-521" << "SIEC");
    ui->settings_hashComboBox->addItems  (QStringList() << "xxhash" << "imohash");

    connect (ui->settings_crocCallTestButton, &QPushButton::clicked, this, &Widget::testCrocExec);
    connect (ui->settings_relayPasswordClearButton, &QPushButton::clicked, ui->settings_relayPasswordField, &QLineEdit::clear);
    connect (ui->settings_applyButton, &QPushButton::clicked, this, &Widget::saveSettings);

    connect (this, &Widget::crocTestFinished, this, &Widget::restoreCrocTestButton, Qt::QueuedConnection);
    connect (ui->settings_proxyTestButton, &QPushButton::clicked, this, &Widget::runProxyTest);
    connect (this, &Widget::proxyTestFinished, this, &Widget::restoreProxyTestButton, Qt::QueuedConnection);
    connect (ui->settings_relayTestButton, &QPushButton::clicked, this, &Widget::runCustomRelayTest);
    connect (this, &Widget::customRelayTestFinished, this, &Widget::restoreCustomRelayTestButton, Qt::QueuedConnection);
    connect (
        ui->settings_historyLimitSlider, &QSlider::valueChanged,
        [&]( int newValue ) {
            if (newValue == 0)
            {
                ui->settings_historyLimitLabel->setText("Disabled");
            }
            else
            {
                ui->settings_historyLimitLabel->setText(QString::number(newValue));
            }
        }
    );
}

Widget::~Widget()
{
    delete ui;
}

void Widget::initSendTab()
{
    ui->send_codePhraseField->setText( Database().getAppSetting(db::CUSTOM_CODE_PHRASE_KEY) );
}

void Widget::initReceiveTab()
{
    Database db;
    ui->receive_TargetDirField->setText(Database::unescape( db.getAppSetting(db::TARGET_SAVE_DIRECTORY) ));
    ui->receive_senderIpField->setText (Database::unescape( db.getAppSetting(db::SENDER_IP) ));
    if (db.getAppSetting(db::ONLY_LOCAL_NETWORK) == "true")
    {
        ui->receive_onlyLocalEnableRadioButton->setChecked(true);
    }
    else
    {
        ui->receive_onlyLocalDisableRadioButton->setChecked(true);
    }
}

void Widget::initSettingsTab()
{
    Database db;
    QString curve = db.getAppSetting( db::ENCRYPTION_CURVE_KEY );
    int curveIndex = ui->settings_curveComboBox->findText (curve);
    ui->settings_curveComboBox->setCurrentIndex (curveIndex);

    QString hash = db.getAppSetting( db::HASH_ALGORITHM_KEY );
    int hashIndex = ui->settings_hashComboBox->findText (hash);
    ui->settings_hashComboBox->setCurrentIndex  (hashIndex);

    db.getAppSetting( db::OVERWRAITING_WITHOUT_PROMT_KEY ) == "true" ?
        ui->settings_overwritingEnableRadioButton->setChecked(true) :
        ui->settings_overwritingDisableRadioButton->setChecked(true);

    ui->settings_crocCallField->setText         (Database::unescape( db.getAppSetting( db::CROC_EXECUTE_COMMAND_KEY ) ));
    ui->settings_proxyAddressField->setText     (Database::unescape( db.getAppSetting( db::PROXY_ADDRESS_KEY ) ));
    ui->settings_relayAddressField->setText     (Database::unescape( db.getAppSetting( db::CUSTOM_RELAY_ADDRESS_KEY ) ));
    ui->settings_historyLimitSlider->setValue   (db.getAppSetting( db::HISTORY_LIMIT_KEY ).toInt());
}

void Widget::saveSettings()
{
    ui->settings_applyButton->setText("Appling");
    QApplication::sync();

    Database db;
    db.setAppSetting (db::CROC_EXECUTE_COMMAND_KEY,       Database::escape( ui->settings_crocCallField->text() ));
    db.setAppSetting (db::PROXY_ADDRESS_KEY,              Database::escape( ui->settings_proxyAddressField->text() ));
    db.setAppSetting (db::CUSTOM_RELAY_ADDRESS_KEY,       Database::escape( ui->settings_relayAddressField->text() ));
    db.setAppSetting (db::HASH_ALGORITHM_KEY,             ui->settings_hashComboBox->currentText());
    db.setAppSetting (db::ENCRYPTION_CURVE_KEY,           ui->settings_curveComboBox->currentText());
    db.setAppSetting (db::OVERWRAITING_WITHOUT_PROMT_KEY, ui->settings_overwritingEnableRadioButton->isChecked() ? "true" : "false");
    db.setAppSetting (db::HISTORY_LIMIT_KEY,              QString::number(ui->settings_historyLimitSlider->value()));

    ui->settings_applyButton->setText("Applied");
    QTimer* timer = new QTimer;
    timer->setInterval(1000);
    timer->setSingleShot(true);
    connect (timer, &QTimer::timeout, [&]() { ui->settings_applyButton->setText("Apply"); });
    connect (timer, &QTimer::timeout, &QTimer::deleteLater);
    timer->start();
}

void Widget::initHistoryTab()
{
    Database db;
    QSqlQuery query (*db.raw());
    if (not query.exec("SELECT rowid,Action_type,Operation_date,File_name,File_path FROM " + db::HISTORY_TABLE + " ORDER BY rowid DESC"))
    {
        qInfo().noquote() << __FUNCTION__ << "select failed:" << query.lastError().text();
    }
    QSqlQueryModel* model = new QSqlQueryModel(this);
    model->setQuery(query);
    ui->history_tableView->setModel(model);
    ui->history_tableView->hideColumn(0); // db rowid
    ui->history_tableView->hideColumn(4); // filepath
}

void Widget::testCrocExec()
{
    QString command = ui->settings_crocCallField->text();
    if (command.isEmpty()) return;

    ui->settings_crocCallTestButton->setText("Testing");
    QApplication::sync();

    CrocExecuteTester* checker = new CrocExecuteTester;
    checker->set(command);

    connect (
        checker, &CrocExecuteTester::finished,
        [=]() {
            emit crocTestFinished(checker->status() ? checker->version().string() : "Not exists");
            checker->deleteLater();
        }
    );

    checker->start();
}

void Widget::restoreCrocTestButton(QString ver)
{
    ui->settings_crocCallTestButton->setText(ver.isEmpty() ? "Failed" : ver);

    QTimer* timer = new QTimer;
    timer->setInterval(2000);
    timer->setSingleShot(true);
    connect (timer, &QTimer::timeout, [=]() { ui->settings_crocCallTestButton->setText("Test"); });
    connect (timer, &QTimer::timeout, &QTimer::deleteLater);
    timer->start();
}

void Widget::checkFilepathToSend()
{
    QString paths = ui->send_filepathLineEdit->text();
    if (paths.isEmpty())
    {
        if (ui->send_sendButton->isEnabled())
        {
            ui->send_sendButton->setEnabled(false);
        }
        return;
    }

    const QString DELIMITER_WORD = "D3L%M!T$R";
    paths.replace("\" \"", DELIMITER_WORD);
    paths.remove('\"');

    bool allFilesExisted = true;
    for (const auto& path: paths.split(DELIMITER_WORD))
    {
        if (not QFile::exists(path))
        {
            allFilesExisted = false;
            break;
        }
    }

    if (not allFilesExisted)
    {
        ui->send_filepathLineEdit->setStyleSheet("color: red");
        if (ui->send_sendButton->isEnabled())
        {
            ui->send_sendButton->setEnabled(false);
        }
        return;
    }

    ui->send_filepathLineEdit->setStyleSheet("");
    ui->send_sendButton->setEnabled(true);
}

void Widget::historyMenuRequest(QPoint pos)
{
    QModelIndex index = ui->history_tableView->indexAt(pos);

    QString filepath = Database::unescape( ui->history_tableView->model()->itemData( index.siblingAtColumn(4) ).first().toString() );
    QString rawPath { filepath };
    if (not QFileInfo(rawPath).isDir())
    {
        rawPath.remove(QRegularExpression("[^/\\\\]*$"));
    }

    QAction* removeFromHistoryAction = new QAction("Remove from history", this);
    removeFromHistoryAction->setIcon(QIcon(":/files/remove.png"));
    connect (
        removeFromHistoryAction, &QAction::triggered,
        [=]() {
            auto map = ui->history_tableView->model()->itemData( index.siblingAtColumn(0) );
            Database().removeFromHistory(map.first().toULongLong());
            initHistoryTab();
        }
    );

    QAction* copyFSpathAction = new QAction("Copy file path", this);
    copyFSpathAction->setIcon(QIcon(":/files/copy.png"));
    connect (
        copyFSpathAction, &QAction::triggered,
        [=]() {
            QApplication::clipboard()->setText(filepath);
        }
    );

    QAction* openFolderAction = new QAction("Open folder", this);
    openFolderAction->setIcon(QIcon(":/files/folder.png"));
    connect (
        openFolderAction, &QAction::triggered,
        [=]() {
            QDesktopServices::openUrl(rawPath);
        }
    );

    if (not QFile::exists(rawPath))
    {
        openFolderAction->setDisabled(true);
    }

    QAction* sendAction = new QAction("Send", this);
    sendAction->setIcon(QIcon(":/files/send.png"));

    if (not QFile::exists(filepath))
    {
        sendAction->setDisabled(true);
    }

    QMenu *menu = new QMenu(this);
    menu->addAction(openFolderAction);
    menu->addAction(copyFSpathAction);
    menu->addAction(sendAction);
    menu->addAction(removeFromHistoryAction);
    menu->popup(ui->history_tableView->viewport()->mapToGlobal(pos));
}

void Widget::runProxyTest()
{
    QString rawAddress = ui->settings_proxyAddressField->text();
    if (not rawAddress.contains(":")) return;

    rawAddress.remove(QRegularExpression("^.*//"));
    rawAddress.remove(QRegularExpression("/*$"));
    bool ipv6 = rawAddress.contains("[");

    QString address {rawAddress};
    if (ipv6)
    {
        address.remove(QRegularExpression("\\]:.*$"));
        address.remove(QRegularExpression("^.*\\["));
    }
    else
    {
        address.remove(QRegularExpression(":.*$"));
    }

    QString portStr {rawAddress};
    portStr.remove(QRegularExpression("^.*:"));
    bool portIsOk = false;
    quint16 port = portStr.toUShort(&portIsOk);
    if (not portIsOk) return;

    ui->settings_proxyTestButton->setText("Testing");
    QApplication::sync();

    SocketChecker* checker = new SocketChecker;
    checker->set(QHostAddress(address), port);
    connect (
        checker, &SocketChecker::finished,
        [=]()
        {
            emit proxyTestFinished(checker->result());
            checker->deleteLater();
        }
    );
    checker->start();
}

void Widget::runCustomRelayTest()
{
    QString rawAddress = ui->settings_relayAddressField->text();
    if (not rawAddress.contains(":")) return;

    rawAddress.remove(QRegularExpression("^.*//"));
    rawAddress.remove(QRegularExpression("/*$"));
    bool ipv6 = rawAddress.contains("[");

    QString address {rawAddress};
    if (ipv6)
    {
        address.remove(QRegularExpression("\\]:.*$"));
        address.remove(QRegularExpression("^.*\\["));
    }
    else
    {
        address.remove(QRegularExpression(":.*$"));
    }

    QString portStr {rawAddress};
    portStr.remove(QRegularExpression("^.*:"));
    bool portIsOk = false;
    quint16 port = portStr.toUShort(&portIsOk);
    if (not portIsOk) return;

    ui->settings_relayTestButton->setText("Testing");
    QApplication::sync();

    SocketChecker* checker = new SocketChecker;
    checker->set(QHostAddress(address), port);
    connect (
        checker, &SocketChecker::finished,
        [=]()
        {
            emit customRelayTestFinished(checker->result());
            checker->deleteLater();
        }
    );
    checker->start();
}

void Widget::restoreCustomRelayTestButton(bool status)
{
    ui->settings_relayTestButton->setText(status ? "Reachable" : "Failed");
    QTimer* timer = new QTimer;
    timer->setInterval(2000);
    timer->setSingleShot(true);
    connect (timer, &QTimer::timeout, [=]() { ui->settings_relayTestButton->setText("Test"); });
    connect (timer, &QTimer::timeout, &QTimer::deleteLater);
    timer->start();
}

void Widget::restoreProxyTestButton(bool status)
{
    ui->settings_proxyTestButton->setText(status ? "Reachable" : "Failed");
    QTimer* timer = new QTimer;
    timer->setInterval(2000);
    timer->setSingleShot(true);
    connect (timer, &QTimer::timeout, [=]() { ui->settings_proxyTestButton->setText("Test"); });
    connect (timer, &QTimer::timeout, &QTimer::deleteLater);
    timer->start();
}
