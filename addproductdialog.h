#ifndef ADDPRODUCTDIALOG_H
#define ADDPRODUCTDIALOG_H

#include <QDialog>

class QLineEdit;
class QComboBox;
class QTextEdit;
class QDoubleSpinBox;
class QDateEdit;
class QCheckBox;

class AddProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddProductDialog(QWidget *parent = nullptr);
    void setEditMode(bool editMode);
    void setProductData(const QString& id, const QString& name, double price,
                        const QString& type, const QDate& expiryDate,
                        const QString& country, bool prescription,
                        const QString& substance, const QString& instructions);

private slots:
    void onAddProduct();
    void onCancel();

private:
    void setupUI();

    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QDoubleSpinBox *priceEdit;
    QComboBox *typeComboBox;
    QDateEdit *expiryDateEdit;
    QLineEdit *countryEdit;
    QCheckBox *prescriptionCheck;
    QLineEdit *substanceEdit;
    QTextEdit *instructionsEdit;

    // Поля для разных типов лекарств
    QWidget *tabletFields;
    QWidget *syrupFields;
    QWidget *ointmentFields;
};

#endif // ADDPRODUCTDIALOG_H
