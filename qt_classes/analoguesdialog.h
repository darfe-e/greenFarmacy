#ifndef ANALOGUESDIALOG_H
#define ANALOGUESDIALOG_H

#include <QDialog>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QCheckBox>
#include <memory>
#include <vector>
#include "my_inheritence/medicine.h"

class AnaloguesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnaloguesDialog(std::shared_ptr<Medicine> currentMedicine,
                             const std::vector<std::shared_ptr<Medicine>>& allMedicines,
                             QWidget *parent = nullptr);

    std::vector<std::string> getSelectedAnalogues() const;

private slots:
    void onAddSelected();
    void onCancel();
    void onMedicineSelectionChanged();

private:
    void setupUI();
    void populateMedicinesList();
    void onSearchTextChanged(const QString &text);

    std::shared_ptr<Medicine> currentMedicine;
    std::vector<std::shared_ptr<Medicine>> allMedicines;
    QListWidget* medicinesList;
    QComboBox* searchComboBox;
    QPushButton* addButton;
    QPushButton* cancelButton;
};

#endif // ANALOGUESDIALOG_H
