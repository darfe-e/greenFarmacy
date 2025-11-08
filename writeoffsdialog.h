#ifndef WRITEOFFSDIALOG_H
#define WRITEOFFSDIALOG_H

#include <QDialog>

class WriteOffsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WriteOffsDialog(QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    void setupUI();
};

#endif // WRITEOFFSDIALOG_H
