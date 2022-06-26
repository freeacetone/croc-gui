#ifndef WIDGET_H
#define WIDGET_H

#include "socketchecker.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void initSettingsTab();
    void saveSettings();
    void initHistoryTab();

    void restoreInfoButton();

    Ui::Widget *ui;

private slots:
    void runProxyTest();
    void restoreProxyTestButton(bool status);

    void runCustomRelayTest();
    void restoreCustomRelayTestButton(bool status);

signals:
    void proxyTestFinished(bool status);
    void customRelayTestFinished(bool status);
};
#endif // WIDGET_H
