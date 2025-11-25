#include "analoguesdialog.h"
#include <QMessageBox>
#include <algorithm>

AnaloguesDialog::AnaloguesDialog(std::shared_ptr<Medicine> currentMedicine,
                                 const std::vector<std::shared_ptr<Medicine>>& allMedicines,
                                 QWidget *parent)
    : QDialog(parent), currentMedicine(currentMedicine), allMedicines(allMedicines)
{
    setupUI();
}

void AnaloguesDialog::setupUI()
{
    setWindowTitle("Добавление аналогов для: " + QString::fromStdString(currentMedicine->getName()));
    setMinimumSize(600, 500);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Заголовок
    QLabel *titleLabel = new QLabel(
        QString("Добавление аналогов для: <b>%1</b> (ID: %2)")
            .arg(QString::fromStdString(currentMedicine->getName()))
            .arg(QString::fromStdString(currentMedicine->getId()))
        );
    titleLabel->setStyleSheet("font-size: 14px; color: #2E7D32; margin: 10px;");
    mainLayout->addWidget(titleLabel);

    // Информация о текущем лекарстве
    QLabel *infoLabel = new QLabel(
        QString("Активное вещество: <b>%1</b>")
            .arg(QString::fromStdString(currentMedicine->getActiveSubstance()))
        );
    infoLabel->setStyleSheet("margin: 5px;");
    mainLayout->addWidget(infoLabel);

    // Поиск
    QHBoxLayout *searchLayout = new QHBoxLayout;
    QLabel *searchLabel = new QLabel("Быстрый поиск:");
    searchComboBox = new QComboBox;
    searchComboBox->setEditable(true);
    searchComboBox->setPlaceholderText("Введите название или ID лекарства...");

    searchLayout->addWidget(searchLabel);
    searchLayout->addWidget(searchComboBox, 1);
    mainLayout->addLayout(searchLayout);

    // Список лекарств
    QLabel *listLabel = new QLabel("Доступные лекарства (отметьте аналоги):");
    mainLayout->addWidget(listLabel);

    medicinesList = new QListWidget;
    medicinesList->setSelectionMode(QListWidget::MultiSelection);
    mainLayout->addWidget(medicinesList, 1);

    // Кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    addButton = new QPushButton("Добавить выбранные");
    cancelButton = new QPushButton("Отмена");

    buttonLayout->addStretch();
    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    // Заполняем списки
    populateMedicinesList();

    // Подключаем сигналы
    connect(addButton, &QPushButton::clicked, this, &AnaloguesDialog::onAddSelected);
    connect(cancelButton, &QPushButton::clicked, this, &AnaloguesDialog::onCancel);
    connect(searchComboBox, &QComboBox::currentTextChanged,
            this, &AnaloguesDialog::onSearchTextChanged);
}

void AnaloguesDialog::onSearchTextChanged(const QString &text)
{
    for (int i = 0; i < medicinesList->count(); ++i) {
        QListWidgetItem *item = medicinesList->item(i);
        bool matches = text.isEmpty() ||
                       item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}

void AnaloguesDialog::populateMedicinesList()
{
    medicinesList->clear();
    searchComboBox->clear();

    // Заполняем список всех лекарств (кроме текущего)
    for (const auto& medicine : allMedicines) {
        if (medicine->getId() == currentMedicine->getId()) {
            continue; // Пропускаем текущее лекарство
        }

        // Добавляем в выпадающий список поиска
        QString displayText = QString("%1 (ID: %2) - %3")
                                  .arg(QString::fromStdString(medicine->getName()))
                                  .arg(QString::fromStdString(medicine->getId()))
                                  .arg(QString::fromStdString(medicine->getActiveSubstance()));
        searchComboBox->addItem(displayText, QString::fromStdString(medicine->getId()));

        // Добавляем в основной список
        QListWidgetItem *item = new QListWidgetItem(displayText);

        // Помечаем уже выбранные аналоги
        auto currentAnalogues = currentMedicine->getAnalogueIds();
        if (std::find(currentAnalogues.begin(), currentAnalogues.end(),
                      medicine->getId()) != currentAnalogues.end()) {
            item->setCheckState(Qt::Checked);
        } else {
            item->setCheckState(Qt::Unchecked);
        }

        // Сохраняем ID в данные элемента
        item->setData(Qt::UserRole, QString::fromStdString(medicine->getId()));
        medicinesList->addItem(item);
    }
}

std::vector<std::string> AnaloguesDialog::getSelectedAnalogues() const
{
    std::vector<std::string> selectedIds;

    for (int i = 0; i < medicinesList->count(); ++i) {
        QListWidgetItem *item = medicinesList->item(i);
        if (item->checkState() == Qt::Checked) {
            QString medicineId = item->data(Qt::UserRole).toString();
            selectedIds.push_back(medicineId.toStdString());
        }
    }

    return selectedIds;
}

void AnaloguesDialog::onAddSelected()
{
    // Проверяем, что выбраны аналоги
    auto selectedIds = getSelectedAnalogues();
    if (selectedIds.empty()) {
        QMessageBox::information(this, "Информация",
                                 "Не выбрано ни одного лекарства в качестве аналога.");
        return;
    }

    accept();
}

void AnaloguesDialog::onCancel()
{
    reject();
}

void AnaloguesDialog::onMedicineSelectionChanged()
{
    // Можно добавить логику для быстрого поиска и выделения
}
