#ifndef LICENSEWINDOW_H
#define LICENSEWINDOW_H

#include <QDialog>

namespace Ui {
class LicenseWindow;
}

class LicenseWindow : public QDialog
{
    Q_OBJECT

public:
    explicit LicenseWindow(QWidget *parent = nullptr);
    ~LicenseWindow();

private:
    Ui::LicenseWindow *ui;
};

#endif // LICENSEWINDOW_H
