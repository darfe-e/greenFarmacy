#ifndef SUPPLIESDIALOG_H
#define SUPPLIESDIALOG_H

#include <QDialog>

class SuppliesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SuppliesDialog(QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    void setupUI();
};

#endif // SUPPLIESDIALOG_H
