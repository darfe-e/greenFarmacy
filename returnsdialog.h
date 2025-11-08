#ifndef RETURNSDIALOG_H
#define RETURNSDIALOG_H

#include <QDialog>

class ReturnsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReturnsDialog(QWidget *parent = nullptr);

private slots:
    void onSave();

private:
    void setupUI();
};

#endif // RETURNSDIALOG_H
