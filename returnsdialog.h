#ifndef RETURNSDIALOG_H
#define RETURNSDIALOG_H

#include <QDialog>
#include <memory>
#include <vector>

class PharmacyManager;
class QTableWidget;
class QLabel;

class ReturnsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReturnsDialog(PharmacyManager& manager, QWidget *parent = nullptr);
    ~ReturnsDialog() = default;

private slots:
    void onClose();

private:
    void setupUI();
    void loadReturnsData();

    PharmacyManager& pharmacyManager;
    QTableWidget* tableWidget;
    QLabel* titleLabel;
};

#endif
