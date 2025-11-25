#include "addproductdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QSpinBox>

// Реализации методов доступа к основным данным
QString AddProductDialog::getId() const
{
    return idEdit ? idEdit->text() : QString();
}

QString AddProductDialog::getName() const
{
    return nameEdit ? nameEdit->text() : QString();
}

double AddProductDialog::getPrice() const
{
    return priceEdit ? priceEdit->value() : 0.0;
}

QString AddProductDialog::getType() const
{
    return typeComboBox ? typeComboBox->currentText() : QString();
}

QDate AddProductDialog::getExpiryDate() const
{
    return expiryDateEdit ? expiryDateEdit->date() : QDate();
}

QString AddProductDialog::getCountry() const
{
    return countryEdit ? countryEdit->text() : QString();
}

bool AddProductDialog::isPrescription() const
{
    return prescriptionCheck ? prescriptionCheck->isChecked() : false;
}

QString AddProductDialog::getSubstance() const
{
    return substanceEdit ? substanceEdit->text() : QString();
}

QString AddProductDialog::getInstructions() const
{
    return instructionsEdit ? instructionsEdit->toPlainText() : QString();
}

// Методы доступа к специфичным данным
int AddProductDialog::getTabletUnits() const
{
    return tabletUnitsEdit ? tabletUnitsEdit->value() : 0;
}

double AddProductDialog::getTabletDosage() const
{
    return tabletDosageEdit ? tabletDosageEdit->value() : 0.0;
}

QString AddProductDialog::getTabletCoating() const
{
    return tabletCoatingEdit ? tabletCoatingEdit->text() : QString();
}

double AddProductDialog::getSyrupVolume() const
{
    return syrupVolumeEdit ? syrupVolumeEdit->value() : 0.0;
}

bool AddProductDialog::getSyrupHasSugar() const
{
    return syrupSugarCheck ? syrupSugarCheck->isChecked() : false;
}

QString AddProductDialog::getSyrupFlavor() const
{
    return syrupFlavorEdit ? syrupFlavorEdit->text() : QString();
}

double AddProductDialog::getOintmentWeight() const
{
    return ointmentWeightEdit ? ointmentWeightEdit->value() : 0.0;
}

QString AddProductDialog::getOintmentBase() const
{
    return ointmentBaseEdit ? ointmentBaseEdit->text() : QString();
}

AddProductDialog::AddProductDialog(QWidget *parent)
    : QDialog(parent)
    , idEdit(nullptr)
    , nameEdit(nullptr)
    , priceEdit(nullptr)
    , typeComboBox(nullptr)
    , expiryDateEdit(nullptr)
    , countryEdit(nullptr)
    , prescriptionCheck(nullptr)
    , substanceEdit(nullptr)
    , instructionsEdit(nullptr)
    , tabletFields(nullptr)
    , syrupFields(nullptr)
    , ointmentFields(nullptr)
    , tabletUnitsEdit(nullptr)
    , tabletDosageEdit(nullptr)
    , tabletCoatingEdit(nullptr)
    , syrupVolumeEdit(nullptr)
    , syrupSugarCheck(nullptr)
    , syrupFlavorEdit(nullptr)
    , ointmentWeightEdit(nullptr)
    , ointmentBaseEdit(nullptr)
    , specificGroup(nullptr)
{
    setupUI();
    QString currentType = typeComboBox ? typeComboBox->currentText() : "Таблетки";
    showSpecificFields(currentType);
}

void AddProductDialog::setEditMode(bool editMode)
{
    if (editMode) {
        setWindowTitle("Редактирование лекарства");
        // Находим и переименовываем кнопку "Добавить" в "Сохранить"
        QPushButton *addButton = findChild<QPushButton*>("addButton");
        if (addButton) {
            addButton->setText("Сохранить");
        }
    } else {
        setWindowTitle("Добавление нового лекарства");
    }
}

// void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
//                                       const QString& type, const QDate& expiryDate,
//                                       const QString& country, bool prescription,
//                                       const QString& substance, const QString& instructions,
//                                       // Добавляем параметры для специфичных полей
//                                       int tabletUnits, double tabletDosage, const QString& tabletCoating,
//                                       double syrupVolume, bool syrupHasSugar, const QString& syrupFlavor,
//                                       double ointmentWeight, const QString& ointmentBase)
// {
//     // Основные поля
//     if (idEdit) idEdit->setText(id);
//     if (nameEdit) nameEdit->setText(name);
//     if (priceEdit) priceEdit->setValue(price);

//     if (typeComboBox) {
//         int index = typeComboBox->findText(type);
//         if (index != -1) {
//             typeComboBox->setCurrentIndex(index);
//             showSpecificFields(type);
//         }
//     }

//     if (expiryDateEdit) expiryDateEdit->setDate(expiryDate);
//     if (countryEdit) countryEdit->setText(country);
//     if (prescriptionCheck) prescriptionCheck->setChecked(prescription);
//     if (substanceEdit) substanceEdit->setText(substance);
//     if (instructionsEdit) instructionsEdit->setPlainText(instructions);

//     // Заполняем специфичные поля в зависимости от типа
//     if (type == "Таблетки") {
//         if (tabletUnitsEdit) tabletUnitsEdit->setValue(tabletUnits);
//         if (tabletDosageEdit) tabletDosageEdit->setValue(tabletDosage);
//         if (tabletCoatingEdit) tabletCoatingEdit->setText(tabletCoating);
//     }
//     else if (type == "Сироп") {
//         if (syrupVolumeEdit) syrupVolumeEdit->setValue(syrupVolume);
//         if (syrupSugarCheck) syrupSugarCheck->setChecked(syrupHasSugar);
//         if (syrupFlavorEdit) syrupFlavorEdit->setText(syrupFlavor);
//     }
//     else if (type == "Мазь") {
//         if (ointmentWeightEdit) ointmentWeightEdit->setValue(ointmentWeight);
//         if (ointmentBaseEdit) ointmentBaseEdit->setText(ointmentBase);
//     }
// }

void AddProductDialog::setupUI()
{
    setWindowTitle("Добавление нового лекарства");
    setMinimumSize(600, 700);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Заголовок
    QLabel *titleLabel = new QLabel("Добавление нового лекарственного препарата");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Основная форма
    QGroupBox *formGroup = new QGroupBox("Основная информация");
    QFormLayout *formLayout = new QFormLayout(formGroup);

    idEdit = new QLineEdit;
    idEdit->setPlaceholderText("Например: M001");
    formLayout->addRow("ID лекарства:", idEdit);

    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("Например: Парацетамол");
    formLayout->addRow("Название:", nameEdit);

    priceEdit = new QDoubleSpinBox;
    priceEdit->setRange(0.01, 10000.0);
    priceEdit->setDecimals(2);
    priceEdit->setSuffix(" руб.");
    formLayout->addRow("Цена:", priceEdit);

    typeComboBox = new QComboBox;
    typeComboBox->addItems({"Таблетки", "Сироп", "Мазь"});
    formLayout->addRow("Тип лекарства:", typeComboBox);

    expiryDateEdit = new QDateEdit;
    expiryDateEdit->setDate(QDate::currentDate().addYears(2));
    expiryDateEdit->setCalendarPopup(true);
    formLayout->addRow("Срок годности:", expiryDateEdit);

    countryEdit = new QLineEdit;
    countryEdit->setPlaceholderText("Например: Россия");
    formLayout->addRow("Страна производитель:", countryEdit);

    prescriptionCheck = new QCheckBox;
    formLayout->addRow("Рецептурный:", prescriptionCheck);

    substanceEdit = new QLineEdit;
    substanceEdit->setPlaceholderText("Например: парацетамол");
    formLayout->addRow("Активное вещество:", substanceEdit);

    instructionsEdit = new QTextEdit;
    instructionsEdit->setMaximumHeight(120); // Увеличена высота
    instructionsEdit->setPlaceholderText("Например: Принимать по 1 таблетке 3 раза в день");
    formLayout->addRow("Инструкция:", instructionsEdit);

    mainLayout->addWidget(formGroup);

    // Группа для специфичных полей
    specificGroup = new QGroupBox("Специфичные параметры");
    QVBoxLayout *specificLayout = new QVBoxLayout(specificGroup);

    // Создаем специфичные поля
    setupTabletFields();
    setupSyrupFields();
    setupOintmentFields();

    // Добавляем все специфичные поля в группу
    specificLayout->addWidget(tabletFields);
    specificLayout->addWidget(syrupFields);
    specificLayout->addWidget(ointmentFields);

    mainLayout->addWidget(specificGroup);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("Добавить");
    addButton->setObjectName("addButton");
    QPushButton *cancelButton = new QPushButton("Отмена");

    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Изначально скрываем все специфичные поля
    hideAllSpecificFields();

    // ⭐ ВАЖНОЕ ИСПРАВЛЕНИЕ: показываем поля для текущего типа при создании
    QString currentType = typeComboBox->currentText();
    showSpecificFields(currentType);

    // Подключаем сигналы
    connect(addButton, &QPushButton::clicked, this, &AddProductDialog::onAddProduct);
    connect(cancelButton, &QPushButton::clicked, this, &AddProductDialog::onCancel);
    connect(typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddProductDialog::onTypeChanged);
}

void AddProductDialog::setupTabletFields()
{
    tabletFields = new QWidget;
    QFormLayout *tabletLayout = new QFormLayout(tabletFields);

    tabletUnitsEdit = new QSpinBox;
    tabletUnitsEdit->setRange(1, 1000);
    tabletUnitsEdit->setValue(20);
    tabletLayout->addRow("Таблеток в упаковке:", tabletUnitsEdit);

    tabletDosageEdit = new QDoubleSpinBox;
    tabletDosageEdit->setRange(1, 2000);
    tabletDosageEdit->setValue(500);
    tabletDosageEdit->setSuffix(" мг");
    tabletLayout->addRow("Дозировка:", tabletDosageEdit);

    tabletCoatingEdit = new QLineEdit;
    tabletCoatingEdit->setPlaceholderText("Например: пленочная, сахарная");
    tabletLayout->addRow("Оболочка:", tabletCoatingEdit);
}

void AddProductDialog::setupSyrupFields()
{
    syrupFields = new QWidget;
    QFormLayout *syrupLayout = new QFormLayout(syrupFields);

    syrupVolumeEdit = new QDoubleSpinBox;
    syrupVolumeEdit->setRange(1, 1000);
    syrupVolumeEdit->setValue(100);
    syrupVolumeEdit->setSuffix(" мл");
    syrupLayout->addRow("Объем:", syrupVolumeEdit);

    syrupSugarCheck = new QCheckBox;
    syrupSugarCheck->setChecked(false);
    syrupLayout->addRow("Содержит сахар:", syrupSugarCheck);

    syrupFlavorEdit = new QLineEdit;
    syrupFlavorEdit->setPlaceholderText("Например: клубничный, малиновый");
    syrupLayout->addRow("Вкус:", syrupFlavorEdit);
}

void AddProductDialog::setupOintmentFields()
{
    ointmentFields = new QWidget;
    QFormLayout *ointmentLayout = new QFormLayout(ointmentFields);

    ointmentWeightEdit = new QDoubleSpinBox;
    ointmentWeightEdit->setRange(1, 1000);
    ointmentWeightEdit->setValue(40);
    ointmentWeightEdit->setSuffix(" г");
    ointmentLayout->addRow("Вес:", ointmentWeightEdit);

    ointmentBaseEdit = new QLineEdit;
    ointmentBaseEdit->setPlaceholderText("Например: гелевая, жировая");
    ointmentLayout->addRow("Тип основы:", ointmentBaseEdit);
}

void AddProductDialog::onTypeChanged(int index)
{
    Q_UNUSED(index);
    QString type = typeComboBox->currentText();
    showSpecificFields(type);
}

void AddProductDialog::showSpecificFields(const QString& type)
{
    hideAllSpecificFields();

    if (type == "Таблетки") {
        tabletFields->setVisible(true);
    } else if (type == "Сироп") {
        syrupFields->setVisible(true);
    } else if (type == "Мазь") {
        ointmentFields->setVisible(true);
    }
}

void AddProductDialog::hideAllSpecificFields()
{
    if (tabletFields) tabletFields->setVisible(false);
    if (syrupFields) syrupFields->setVisible(false);
    if (ointmentFields) ointmentFields->setVisible(false);
}

void AddProductDialog::onAddProduct()
{
    // Проверяем обязательные основные поля
    if (idEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите ID лекарства");
        return;
    }

    if (nameEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите название лекарства");
        return;
    }

    if (priceEdit->value() <= 0) {
        QMessageBox::warning(this, "Ошибка", "Введите корректную цену");
        return;
    }

    if (substanceEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите активное вещество");
        return;
    }

    // Проверяем специфичные поля в зависимости от типа
    QString type = typeComboBox->currentText();
    if (type == "Таблетки") {
        if (tabletCoatingEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите тип оболочки для таблеток");
            return;
        }
    } else if (type == "Сироп") {
        if (syrupFlavorEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите вкус сиропа");
            return;
        }
    } else if (type == "Мазь") {
        if (ointmentBaseEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Ошибка", "Введите тип основы мази");
            return;
        }
    }

    accept();
}

void AddProductDialog::onCancel()
{
    reject();
}

// Основной метод со всеми параметрами
void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      int tabletUnits, double tabletDosage, const QString& tabletCoating,
                                      double syrupVolume, bool syrupHasSugar, const QString& syrupFlavor,
                                      double ointmentWeight, const QString& ointmentBase)
{
    // Основные поля
    if (idEdit) idEdit->setText(id);
    if (nameEdit) nameEdit->setText(name);
    if (priceEdit) priceEdit->setValue(price);

    if (typeComboBox) {
        int index = typeComboBox->findText(type);
        if (index != -1) {
            typeComboBox->setCurrentIndex(index);
            // ВАЖНО: показываем специфичные поля ДО заполнения данных
            showSpecificFields(type);
        }
    }

    if (expiryDateEdit) expiryDateEdit->setDate(expiryDate);
    if (countryEdit) countryEdit->setText(country);
    if (prescriptionCheck) prescriptionCheck->setChecked(prescription);
    if (substanceEdit) substanceEdit->setText(substance);
    if (instructionsEdit) instructionsEdit->setPlainText(instructions);

    // Заполняем ВСЕ специфичные поля (они будут скрыты/показаны автоматически)
    if (tabletUnitsEdit) tabletUnitsEdit->setValue(tabletUnits);
    if (tabletDosageEdit) tabletDosageEdit->setValue(tabletDosage);
    if (tabletCoatingEdit) tabletCoatingEdit->setText(tabletCoating);
    if (syrupVolumeEdit) syrupVolumeEdit->setValue(syrupVolume);
    if (syrupSugarCheck) syrupSugarCheck->setChecked(syrupHasSugar);
    if (syrupFlavorEdit) syrupFlavorEdit->setText(syrupFlavor);
    if (ointmentWeightEdit) ointmentWeightEdit->setValue(ointmentWeight);
    if (ointmentBaseEdit) ointmentBaseEdit->setText(ointmentBase);
}

// Метод только для основных полей
void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions)
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   0, 0.0, "",    // tablet parameters
                   0.0, false, "", // syrup parameters
                   0.0, "");      // ointment parameters
}

// Метод для таблеток
void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      int tabletUnits, double tabletDosage, const QString& tabletCoating)
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   tabletUnits, tabletDosage, tabletCoating,
                   0.0, false, "",  // syrup parameters
                   0.0, "");        // ointment parameters
}

// Метод для сиропа
void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      double syrupVolume, bool syrupHasSugar, const QString& syrupFlavor)
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   0, 0.0, "",    // tablet parameters
                   syrupVolume, syrupHasSugar, syrupFlavor,
                   0.0, "");      // ointment parameters
}

// Метод для мази
void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      double ointmentWeight, const QString& ointmentBase)
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   0, 0.0, "",    // tablet parameters
                   0.0, false, "", // syrup parameters
                   ointmentWeight, ointmentBase);
}
