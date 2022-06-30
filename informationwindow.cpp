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

#include "informationwindow.h"
#include "ui_informationwindow.h"
#include "crocexecutetester.h"
#include "database.h"
#include "latestversiongithubchecker.h"
#include "g.h"

#include <QDesktopServices>
#include <QApplication>
#include <QClipboard>
#include <QTimer>
#include <QIcon>
#include <QUrl>

InformationWindow::InformationWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InformationWindow)
{
    ui->setupUi(this);

    setWindowIcon(QIcon(":/files/info.png"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Information");

    ui->mainLabel->setText(
        "Croc CLI - software for send things from one computer to another.\n"
        "Croc GUI is additional tool for it, written in C++/Qt5."
    );

    ui->gui_currentVersionLabel->setText(g::VERSION.string());

    CrocExecuteTester* crocCli = new CrocExecuteTester;
    crocCli->set(Database().getAppSetting(db::CROC_EXECUTE_COMMAND_KEY));
    connect (
        crocCli, &CrocExecuteTester::finished,
        [=]() {
            ui->cli_currentVersionLabel->setText(crocCli->status() ? crocCli->version().string() : "Not exists");
            crocCli->deleteLater();
        }
    );
    crocCli->start();

    LatestVersionGitHubChecker* cliLatestChecker = new LatestVersionGitHubChecker;
    cliLatestChecker->setRepo("schollz/croc");
    connect (
        cliLatestChecker, &LatestVersionGitHubChecker::finished,
        [=]() {
            ui->cli_actualVersionLabel->setText (
                                                    cliLatestChecker->status() ?
                                                    cliLatestChecker->version().string() :
                                                    cliLatestChecker->errorString()
                                                );
            cliLatestChecker->deleteLater();
        }
    );
    cliLatestChecker->start();

    LatestVersionGitHubChecker* guiLatestChecker = new LatestVersionGitHubChecker;
    guiLatestChecker->setRepo("freeacetone/croc-gui");
    connect (
        guiLatestChecker, &LatestVersionGitHubChecker::finished,
        [=]() {
            ui->gui_actualVersionLabel->setText (
                                                    guiLatestChecker->status() ?
                                                    guiLatestChecker->version().string() :
                                                    guiLatestChecker->errorString()
                                                );
            guiLatestChecker->deleteLater();
        }
    );
    guiLatestChecker->start();


    connect (
        ui->gui_repoButton, &QPushButton::clicked,
        [&]() {
            QDesktopServices::openUrl(QUrl("https://github.com/freeacetone/croc-gui/releases"));
        }
    );
    connect (
        ui->cli_repoButton, &QPushButton::clicked,
        [&]() {
            QDesktopServices::openUrl(QUrl("https://github.com/schollz/croc/releases"));
        }
    );

    ui->copyBitcoinButton->setFocus();
    connect (
        ui->copyBitcoinButton, &QPushButton::clicked,
        [&]() {
            QApplication::clipboard()->setText(ui->bitcoin->text());
            ui->copyBitcoinButton->setText("Copied!");
            ui->copyBitcoinButton->setIcon(QIcon(":/files/heart.png"));
            QTimer* timer = new QTimer;
            timer->setSingleShot(true);
            timer->setInterval(2000);
            connect (
                timer, &QTimer::timeout,
                [=]() {
                     ui->copyBitcoinButton->setText("Copy");
                     ui->copyBitcoinButton->setIcon(QIcon(":/files/bitcoin.png"));
                     timer->deleteLater();
                }
            );
            timer->start();
        }
    );
    connect (
        ui->copyMoneroButton, &QPushButton::clicked,
        [&]() {
            QApplication::clipboard()->setText(ui->monero->text());
            ui->copyMoneroButton->setText("Copied!");
            ui->copyMoneroButton->setIcon(QIcon(":/files/heart.png"));
            QTimer* timer = new QTimer;
            timer->setSingleShot(true);
            timer->setInterval(2000);
            connect (
                timer, &QTimer::timeout,
                [=]() {
                     ui->copyMoneroButton->setText("Copy");
                     ui->copyMoneroButton->setIcon(QIcon(":/files/monero.png"));
                     timer->deleteLater();
                }
            );
            timer->start();
        }
    );
}

InformationWindow::~InformationWindow()
{
    delete ui;
}
