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
#include <QSpinBox>
#include <QGroupBox>

class AddProductDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddProductDialog(QWidget *parent = nullptr);
    void setEditMode(bool editMode);
    void setProductData(const QString& id, const QString& name, double price,
                        const QString& type, const QDate& expiryDate,
                        const QString& country, bool prescription,
                        const QString& substance, const QString& instructions,
                        // Добавляем параметры для специфичных полей
                        int tabletUnits = 0, double tabletDosage = 0.0, const QString& tabletCoating = "",
                        double syrupVolume = 0.0, bool syrupHasSugar = false, const QString& syrupFlavor = "",
                        double ointmentWeight = 0.0, const QString& ointmentBase = "");

    // Методы доступа к основным данным
    QString getId() const;
    QString getName() const;
    double getPrice() const;
    QString getType() const;
    QDate getExpiryDate() const;
    QString getCountry() const;
    bool isPrescription() const;
    QString getSubstance() const;
    QString getInstructions() const;

    // Методы доступа к специфичным данным
    int getTabletUnits() const;
    double getTabletDosage() const;
    QString getTabletCoating() const;

    double getSyrupVolume() const;
    bool getSyrupHasSugar() const;
    QString getSyrupFlavor() const;

    double getOintmentWeight() const;
    QString getOintmentBase() const;

private slots:
    void onAddProduct();
    void onCancel();
    void onTypeChanged(int index);

private:
    void setupUI();
    void setupTabletFields();
    void setupSyrupFields();
    void setupOintmentFields();
    void showSpecificFields(const QString& type);
    void hideAllSpecificFields();

    // Указатели на виджеты основной формы
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QDoubleSpinBox *priceEdit;
    QComboBox *typeComboBox;
    QDateEdit *expiryDateEdit;
    QLineEdit *countryEdit;
    QCheckBox *prescriptionCheck;
    QLineEdit *substanceEdit;
    QTextEdit *instructionsEdit;

    // Контейнеры для специфичных полей
    QWidget *tabletFields;
    QWidget *syrupFields;
    QWidget *ointmentFields;

    // Специфичные поля для таблеток
    QSpinBox *tabletUnitsEdit;
    QDoubleSpinBox *tabletDosageEdit;
    QLineEdit *tabletCoatingEdit;

    // Специфичные поля для сиропа
    QDoubleSpinBox *syrupVolumeEdit;
    QCheckBox *syrupSugarCheck;
    QLineEdit *syrupFlavorEdit;

    // Специфичные поля для мази
    QDoubleSpinBox *ointmentWeightEdit;
    QLineEdit *ointmentBaseEdit;

    // Группа для специфичных полей
    QGroupBox *specificGroup;
};

#endif // ADDPRODUCTDIALOG_H
