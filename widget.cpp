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

#include "widget.h"
#include "ui_widget.h"
#include "database.h"

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

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("Croc GUI");
    setWindowIcon(QIcon(":/files/crocodile.png"));

    restoreInfoButton();
    ui->progressBar->setVisible(false);
    connect (
        ui->tabWidget, &QTabWidget::tabBarClicked,
        [=]( int index ) {
            if (index == 3)
            {
                initSettingsTab();
            }
            else if (index == 2)
            {
                initHistoryTab();
            }
        }
    );

    // History

    ui->history_tableView->verticalHeader()->setSectionsClickable(false);
    ui->history_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect (ui->history_tableView, &QTableView::customContextMenuRequested, this, &Widget::historyMenuRequest);

    // Settings

    ui->settings_curveComboBox->addItems (QStringList() << "P-256" << "P-348" << "P-521" << "SIEC");
    ui->settings_hashComboBox->addItems  (QStringList() << "xxhash" << "imohash");

    connect (ui->settings_codePhraseClearButton, &QPushButton::clicked,
             ui->settings_codePhraseField,       &QLineEdit::clear);

    connect (ui->settings_applyButton, &QPushButton::clicked, this, &Widget::saveSettings);

    connect (
        ui->settings_historyLimitSlider, &QSlider::valueChanged,
        [=]( int newValue ) {
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

    connect (ui->settings_proxyTestButton, &QPushButton::clicked, this, &Widget::runProxyTest);
    connect (this, &Widget::proxyTestFinished, this, &Widget::restoreProxyTestButton, Qt::QueuedConnection);

    connect (ui->settings_relayTestButton, &QPushButton::clicked, this, &Widget::runCustomRelayTest);
    connect (this, &Widget::customRelayTestFinished, this, &Widget::restoreCustomRelayTestButton, Qt::QueuedConnection);
}

Widget::~Widget()
{
    delete ui;
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

    ui->settings_codePhraseField->setText       (Database::unescape( db.getAppSetting( db::CUSTOM_CODE_PHRASE_KEY ) ));
    ui->settings_proxyAddressField->setText     (Database::unescape( db.getAppSetting( db::PROXY_ADDRESS_KEY ) ));
    ui->settings_relayAddressField->setText     (Database::unescape( db.getAppSetting( db::CUSTOM_RELAY_ADDRESS_KEY ) ));
    ui->settings_historyLimitSlider->setValue   (db.getAppSetting( db::HISTORY_LIMIT_KEY ).toInt());
}

void Widget::saveSettings()
{
    ui->settings_applyButton->setText("Appling");
    QApplication::sync();

    Database db;
    db.setAppSetting (db::PROXY_ADDRESS_KEY,              Database::escape( ui->settings_proxyAddressField->text() ));
    db.setAppSetting (db::CUSTOM_RELAY_ADDRESS_KEY,       Database::escape( ui->settings_relayAddressField->text() ));
    db.setAppSetting (db::CUSTOM_CODE_PHRASE_KEY,         Database::escape( ui->settings_codePhraseField->text() ));
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

void Widget::restoreInfoButton()
{
    ui->info_button->setText("GitHub");
    ui->info_button->setIcon(QIcon(":/files/github.ico"));
    connect (ui->info_button, &QPushButton::clicked,
             [&]() { QDesktopServices::openUrl(QUrl("https://github.com/freeacetone/croc-gui")); });
}

void Widget::historyMenuRequest(QPoint pos)
{
    QModelIndex index = ui->history_tableView->indexAt(pos);

    QString filepath = ui->history_tableView->model()->itemData( index.siblingAtColumn(4) ).first().toString();
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
