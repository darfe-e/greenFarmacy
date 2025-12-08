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

QString AddProductDialog::getId() const                       // Получение ID лекарства
{
    return idEdit ? idEdit->text() : QString();
}

QString AddProductDialog::getName() const                     // Получение названия лекарства
{
    return nameEdit ? nameEdit->text() : QString();
}

double AddProductDialog::getPrice() const                     // Получение цены лекарства
{
    return priceEdit ? priceEdit->value() : 0.0;
}

QString AddProductDialog::getType() const                     // Получение типа лекарства
{
    return typeComboBox ? typeComboBox->currentText() : QString();
}

QDate AddProductDialog::getExpiryDate() const                 // Получение срока годности
{
    return expiryDateEdit ? expiryDateEdit->date() : QDate();
}

QString AddProductDialog::getCountry() const                  // Получение страны производителя
{
    return countryEdit ? countryEdit->text() : QString();
}

bool AddProductDialog::isPrescription() const                 // Проверка рецептурности
{
    return prescriptionCheck ? prescriptionCheck->isChecked() : false;
}

QString AddProductDialog::getSubstance() const                // Получение активного вещества
{
    return substanceEdit ? substanceEdit->text() : QString();
}

QString AddProductDialog::getInstructions() const             // Получение инструкции
{
    return instructionsEdit ? instructionsEdit->toPlainText() : QString();
}

int AddProductDialog::getTabletUnits() const                  // Получение количества таблеток
{
    return tabletUnitsEdit ? tabletUnitsEdit->value() : 0;
}

double AddProductDialog::getTabletDosage() const              // Получение дозировки таблеток
{
    return tabletDosageEdit ? tabletDosageEdit->value() : 0.0;
}

QString AddProductDialog::getTabletCoating() const            // Получение типа оболочки таблеток
{
    return tabletCoatingEdit ? tabletCoatingEdit->text() : QString();
}

double AddProductDialog::getSyrupVolume() const               // Получение объема сиропа
{
    return syrupVolumeEdit ? syrupVolumeEdit->value() : 0.0;
}

bool AddProductDialog::getSyrupHasSugar() const               // Проверка наличия сахара в сиропе
{
    return syrupSugarCheck ? syrupSugarCheck->isChecked() : false;
}

QString AddProductDialog::getSyrupFlavor() const              // Получение вкуса сиропа
{
    return syrupFlavorEdit ? syrupFlavorEdit->text() : QString();
}

double AddProductDialog::getOintmentWeight() const            // Получение веса мази
{
    return ointmentWeightEdit ? ointmentWeightEdit->value() : 0.0;
}

QString AddProductDialog::getOintmentBase() const             // Получение типа основы мази
{
    return ointmentBaseEdit ? ointmentBaseEdit->text() : QString();
}

AddProductDialog::AddProductDialog(QWidget *parent)          // Конструктор диалога
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

void AddProductDialog::setEditMode(bool editMode)            // Установка режима редактирования
{
    if (editMode)
    {
        setWindowTitle("Редактирование лекарства");
        QPushButton *addButton = findChild<QPushButton*>("addButton");
        if (addButton)
            addButton->setText("Сохранить");
    }
    else
        setWindowTitle("Добавление нового лекарства");
}

void AddProductDialog::setupUI()                             // Настройка пользовательского интерфейса
{
    setWindowTitle("Добавление нового лекарства");
    setMinimumSize(600, 700);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Добавление нового лекарственного препарата");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

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
    instructionsEdit->setMaximumHeight(120);
    instructionsEdit->setPlaceholderText("Например: Принимать по 1 таблетке 3 раза в день");
    formLayout->addRow("Инструкция:", instructionsEdit);

    mainLayout->addWidget(formGroup);

    specificGroup = new QGroupBox("Специфичные параметры");
    QVBoxLayout *specificLayout = new QVBoxLayout(specificGroup);

    setupTabletFields();
    setupSyrupFields();
    setupOintmentFields();

    specificLayout->addWidget(tabletFields);
    specificLayout->addWidget(syrupFields);
    specificLayout->addWidget(ointmentFields);

    mainLayout->addWidget(specificGroup);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("Добавить");
    addButton->setObjectName("addButton");
    QPushButton *cancelButton = new QPushButton("Отмена");

    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    hideAllSpecificFields();

    QString currentType = typeComboBox->currentText();
    showSpecificFields(currentType);

    connect(addButton, &QPushButton::clicked, this, &AddProductDialog::onAddProduct);
    connect(cancelButton, &QPushButton::clicked, this, &AddProductDialog::onCancel);
    connect(typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &AddProductDialog::onTypeChanged);
}

void AddProductDialog::setupTabletFields()                   // Настройка полей для таблеток
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

void AddProductDialog::setupSyrupFields()                    // Настройка полей для сиропа
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

void AddProductDialog::setupOintmentFields()                 // Настройка полей для мази
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

void AddProductDialog::onTypeChanged(int index)               // Обработка изменения типа лекарства
{
    Q_UNUSED(index);
    QString type = typeComboBox->currentText();
    showSpecificFields(type);
}

void AddProductDialog::showSpecificFields(const QString& type) // Показать специфичные поля для типа
{
    hideAllSpecificFields();

    if (type == "Таблетки")
        tabletFields->setVisible(true);
    else if (type == "Сироп")
        syrupFields->setVisible(true);
    else if (type == "Мазь")
        ointmentFields->setVisible(true);
}

void AddProductDialog::hideAllSpecificFields()               // Скрыть все специфичные поля
{
    if (tabletFields) tabletFields->setVisible(false);
    if (syrupFields) syrupFields->setVisible(false);
    if (ointmentFields) ointmentFields->setVisible(false);
}

void AddProductDialog::onAddProduct()                        // Обработка нажатия кнопки "Добавить"
{
    if (idEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Введите ID лекарства");
        return;
    }

    if (nameEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Введите название лекарства");
        return;
    }

    if (priceEdit->value() <= 0)
    {
        QMessageBox::warning(this, "Ошибка", "Введите корректную цену");
        return;
    }

    if (substanceEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Ошибка", "Введите активное вещество");
        return;
    }

    QString type = typeComboBox->currentText();
    if (type == "Таблетки")
    {
        if (tabletCoatingEdit->text().isEmpty())
        {
            QMessageBox::warning(this, "Ошибка", "Введите тип оболочки для таблеток");
            return;
        }
    }
    else if (type == "Сироп")
    {
        if (syrupFlavorEdit->text().isEmpty())
        {
            QMessageBox::warning(this, "Ошибка", "Введите вкус сиропа");
            return;
        }
    }
    else if (type == "Мазь")
    {
        if (ointmentBaseEdit->text().isEmpty())
        {
            QMessageBox::warning(this, "Ошибка", "Введите тип основы мази");
            return;
        }
    }

    accept();
}

void AddProductDialog::onCancel()                            // Обработка нажатия кнопки "Отмена"
{
    reject();
}

void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      int tabletUnits, double tabletDosage, const QString& tabletCoating,
                                      double syrupVolume, bool syrupHasSugar, const QString& syrupFlavor,
                                      double ointmentWeight, const QString& ointmentBase) // Установка всех данных
{
    if (idEdit) idEdit->setText(id);
    if (nameEdit) nameEdit->setText(name);
    if (priceEdit) priceEdit->setValue(price);

    if (typeComboBox)
    {
        int index = typeComboBox->findText(type);
        if (index != -1)
        {
            typeComboBox->setCurrentIndex(index);
            showSpecificFields(type);
        }
    }

    if (expiryDateEdit) expiryDateEdit->setDate(expiryDate);
    if (countryEdit) countryEdit->setText(country);
    if (prescriptionCheck) prescriptionCheck->setChecked(prescription);
    if (substanceEdit) substanceEdit->setText(substance);
    if (instructionsEdit) instructionsEdit->setPlainText(instructions);

    if (tabletUnitsEdit) tabletUnitsEdit->setValue(tabletUnits);
    if (tabletDosageEdit) tabletDosageEdit->setValue(tabletDosage);
    if (tabletCoatingEdit) tabletCoatingEdit->setText(tabletCoating);
    if (syrupVolumeEdit) syrupVolumeEdit->setValue(syrupVolume);
    if (syrupSugarCheck) syrupSugarCheck->setChecked(syrupHasSugar);
    if (syrupFlavorEdit) syrupFlavorEdit->setText(syrupFlavor);
    if (ointmentWeightEdit) ointmentWeightEdit->setValue(ointmentWeight);
    if (ointmentBaseEdit) ointmentBaseEdit->setText(ointmentBase);
}

void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions) // Установка основных данных
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   0, 0.0, "",
                   0.0, false, "",
                   0.0, "");
}

void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      int tabletUnits, double tabletDosage, const QString& tabletCoating) // Данные для таблеток
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   tabletUnits, tabletDosage, tabletCoating,
                   0.0, false, "",
                   0.0, "");
}

void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      double syrupVolume, bool syrupHasSugar, const QString& syrupFlavor) // Данные для сиропа
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   0, 0.0, "",
                   syrupVolume, syrupHasSugar, syrupFlavor,
                   0.0, "");
}

void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions,
                                      double ointmentWeight, const QString& ointmentBase) // Данные для мази
{
    setProductData(id, name, price, type, expiryDate, country, prescription,
                   substance, instructions,
                   0, 0.0, "",
                   0.0, false, "",
                   ointmentWeight, ointmentBase);
}
