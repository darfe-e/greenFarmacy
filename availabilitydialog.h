#ifndef AVAILABILITYDIALOG_H
#define AVAILABILITYDIALOG_H

#include <QDialog>

class AvailabilityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AvailabilityDialog(QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    void setupUI();
};

#endif // AVAILABILITYDIALOG_H
