#ifndef OPERATIONSDIALOG_H
#define OPERATIONSDIALOG_H

#include <QDialog>
#include <memory>
#include <vector>
#include <string>

class PharmacyManager;
class QTableWidget;
class QLabel;
class QComboBox;

class OperationsDialog : public QDialog
{
    Q_OBJECT

public:
    enum OperationType {
        SUPPLY,
        RETURN,
        WRITEOFF
    };

    OperationsDialog(PharmacyManager& manager, OperationType type, QWidget *parent = nullptr);
    ~OperationsDialog() = default;

private slots:
    void onClose();
    void onOperationTypeChanged(int index);

private:
    void setupUI();
    void loadOperationsData();
    void updateTableColumns();
    QString getOperationTypeString() const;
    QString getWindowTitle() const;

    PharmacyManager& pharmacyManager;
    OperationType currentType;
    QTableWidget* tableWidget;
    QLabel* titleLabel;
    QComboBox* typeComboBox;
};

#endif // OPERATIONSDIALOG_H
