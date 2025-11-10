#ifndef ADDPRODUCTDIALOG_H
#define ADDPRODUCTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QDate>

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

    // Методы доступа к данным
    QString getId() const;
    QString getName() const;
    double getPrice() const;
    QString getType() const;
    QDate getExpiryDate() const;
    QString getCountry() const;
    bool isPrescription() const;
    QString getSubstance() const;
    QString getInstructions() const;

private slots:
    void onAddProduct();
    void onCancel();

private:
    void setupUI();

    // Указатели на виджеты формы
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
