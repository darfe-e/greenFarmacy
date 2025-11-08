#ifndef ANALOGUESDIALOG_H
#define ANALOGUESDIALOG_H

#include <QDialog>

class AnaloguesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnaloguesDialog(QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    void setupUI();
};

#endif // ANALOGUESDIALOG_H
