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

AddProductDialog::AddProductDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void AddProductDialog::setEditMode(bool editMode)
{
    if (editMode) {
        setWindowTitle("Редактирование лекарства");
        // Можно заблокировать поле ID при редактировании
        // idEdit->setReadOnly(true);
    } else {
        setWindowTitle("Добавление нового лекарства");
    }
}

void AddProductDialog::setProductData(const QString& id, const QString& name, double price,
                                      const QString& type, const QDate& expiryDate,
                                      const QString& country, bool prescription,
                                      const QString& substance, const QString& instructions)
{
    idEdit->setText(id);
    nameEdit->setText(name);
    priceEdit->setValue(price);

    int index = typeComboBox->findText(type);
    if (index != -1) {
        typeComboBox->setCurrentIndex(index);
    }

    expiryDateEdit->setDate(expiryDate);
    countryEdit->setText(country);
    prescriptionCheck->setChecked(prescription);
    substanceEdit->setText(substance);
    instructionsEdit->setPlainText(instructions);
}

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
    instructionsEdit->setMaximumHeight(80);
    instructionsEdit->setPlaceholderText("Например: Принимать по 1 таблетке 3 раза в день");
    formLayout->addRow("Инструкция:", instructionsEdit);

    mainLayout->addWidget(formGroup);

    // Группа для специфичных полей (пока пустая)
    QGroupBox *specificGroup = new QGroupBox("Специфичные параметры");
    QVBoxLayout *specificLayout = new QVBoxLayout(specificGroup);

    QLabel *specificLabel = new QLabel("Выберите тип лекарства для отображения дополнительных полей");
    specificLabel->setAlignment(Qt::AlignCenter);
    specificLayout->addWidget(specificLabel);

    mainLayout->addWidget(specificGroup);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *addButton = new QPushButton("Добавить");
    QPushButton *cancelButton = new QPushButton("Отмена");

    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Подключаем сигналы
    connect(addButton, &QPushButton::clicked, this, &AddProductDialog::onAddProduct);
    connect(cancelButton, &QPushButton::clicked, this, &AddProductDialog::onCancel);
}

void AddProductDialog::onAddProduct()
{
    QMessageBox::information(this, "Добавление", "Лекарство добавлено в систему (заглушка)");
    accept();
}

void AddProductDialog::onCancel()
{
    reject();
}
