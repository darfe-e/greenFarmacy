#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QListWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QStringListModel>
#include <QKeySequence>
#include <QShortcut>
#include "addproductdialog.h"
#include "suppliesdialog.h"
#include "returnsdialog.h"
#include "writeoffsdialog.h"
#include "analoguesdialog.h"
#include "availabilitydialog.h"
#include "Exception/PharmacyExceptions/InvalidProductIdException.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , currentProduct(nullptr)
    , searchEdit(nullptr)
    , productsList(nullptr)
    , contentText(nullptr)
    , searchCompleter(new QCompleter(this))
    , isEditMode(false)
{
    setupUI();
    setupColors();
    loadAllData();
    updateCompleter();
    hideEditPanel(); // Скрываем панель редактирования initially
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    setWindowTitle("Зеленая Аптека - Система управления");
    setMinimumSize(1400, 900);

    // Центральный виджет
    QWidget *centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    // Основной layout
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setSpacing(10);

    // === ЛЕВАЯ ПАНЕЛЬ ===
    QWidget *leftPanel = new QWidget;
    leftPanel->setFixedWidth(300);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(15);

    // Поиск
    QGroupBox *searchGroup = new QGroupBox("Поиск лекарств");
    QVBoxLayout *searchLayout = new QVBoxLayout(searchGroup);

    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("Введите название, страну или ID...");
    searchCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    searchEdit->setCompleter(searchCompleter);

    searchButton = new QPushButton("Найти");
    showAllButton = new QPushButton("Показать все");

    searchLayout->addWidget(searchEdit);
    searchLayout->addWidget(searchButton);
    searchLayout->addWidget(showAllButton);

    // Навигация - УБРАНА КНОПКА "АНАЛОГИ"
    QGroupBox *navGroup = new QGroupBox("Навигация");
    QVBoxLayout *navLayout = new QVBoxLayout(navGroup);

    suppliesBtn = new QPushButton("Поставки");
    returnsBtn = new QPushButton("Возвраты");
    writeOffsBtn = new QPushButton("Списания");
    availabilityBtn = new QPushButton("Наличие в аптеках");
    addProductBtn = new QPushButton("Добавить лекарство");

    navLayout->addWidget(suppliesBtn);
    navLayout->addWidget(returnsBtn);
    navLayout->addWidget(writeOffsBtn);
    navLayout->addWidget(availabilityBtn);
    navLayout->addWidget(addProductBtn);

    // Действия
    QGroupBox *actionsGroup = new QGroupBox("Действия");
    QVBoxLayout *actionsLayout = new QVBoxLayout(actionsGroup);

    saveBtn = new QPushButton("Сохранить изменения");
    undoBtn = new QPushButton("Отменить (Ctrl+Z)");

    actionsLayout->addWidget(saveBtn);
    actionsLayout->addWidget(undoBtn);

    leftLayout->addWidget(searchGroup);
    leftLayout->addWidget(navGroup);
    leftLayout->addWidget(actionsGroup);
    leftLayout->addStretch();

    // === ЦЕНТРАЛЬНАЯ ПАНЕЛЬ ===
    QWidget *centerPanel = new QWidget;
    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);

    QLabel *contentLabel = new QLabel("Информация");
    contentLabel->setAlignment(Qt::AlignCenter);
    contentLabel->setStyleSheet("font-size: 18px; font-weight: bold; margin: 10px;");

    contentText = new QTextEdit;
    contentText->setReadOnly(true);
    contentText->setPlainText("Используйте поиск или нажмите 'Показать все' для отображения лекарств.");

    productsList = new QListWidget;
    productsList->setVisible(false); // Изначально скрыт

    centerLayout->addWidget(contentLabel);
    centerLayout->addWidget(productsList);
    centerLayout->addWidget(contentText, 1); // contentText растягивается

    // === ПРАВАЯ ПАНЕЛЬ (редактирование) ===
    editPanel = new QWidget;
    editPanel->setFixedWidth(250);
    QVBoxLayout *editLayout = new QVBoxLayout(editPanel);

    QLabel *editLabel = new QLabel("Управление");
    editLabel->setAlignment(Qt::AlignCenter);
    editLabel->setStyleSheet("font-size: 16px; font-weight: bold; margin: 10px;");

    editBtn = new QPushButton("Изменить");
    deleteBtn = new QPushButton("Удалить");
    addAnalogueBtn = new QPushButton("Добавить аналог");
    viewAnaloguesBtn = new QPushButton("Просмотр аналогов"); // НОВАЯ КНОПКА В ПРАВОЙ ПАНЕЛИ

    editLayout->addWidget(editLabel);
    editLayout->addWidget(editBtn);
    editLayout->addWidget(deleteBtn);
    editLayout->addWidget(addAnalogueBtn);
    editLayout->addWidget(viewAnaloguesBtn); // ДОБАВЛЕНА В ПРАВУЮ ПАНЕЛЬ
    editLayout->addStretch();

    // Добавляем панели в основной layout
    mainLayout->addWidget(leftPanel);
    mainLayout->addWidget(centerPanel, 1); // Центральная панель растягивается
    mainLayout->addWidget(editPanel);

    // Подключаем сигналы
    connect(searchEdit, &QLineEdit::textChanged, this, &MainWindow::onSearchTextChanged);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);
    connect(showAllButton, &QPushButton::clicked, this, &MainWindow::onShowAllProducts);
    connect(suppliesBtn, &QPushButton::clicked, this, &MainWindow::onShowSupplies);
    connect(returnsBtn, &QPushButton::clicked, this, &MainWindow::onShowReturns);
    connect(writeOffsBtn, &QPushButton::clicked, this, &MainWindow::onShowWriteOffs);
    connect(availabilityBtn, &QPushButton::clicked, this, &MainWindow::onShowPharmacyAvailability);
    connect(addProductBtn, &QPushButton::clicked, this, &MainWindow::onAddProduct);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveChanges);
    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::onUndo);
    connect(productsList, &QListWidget::itemClicked, this, &MainWindow::onProductSelected);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditProduct);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteProduct);
    connect(addAnalogueBtn, &QPushButton::clicked, this, &MainWindow::onAddAnalogue);
    connect(viewAnaloguesBtn, &QPushButton::clicked, this, &MainWindow::onShowAnalogues);
    connect(productsList, &QListWidget::itemSelectionChanged, this, &MainWindow::onItemSelected);

    // Горячие клавиши
    undoBtn->setShortcut(QKeySequence("Ctrl+Z"));
    QShortcut *searchShortcut = new QShortcut(QKeySequence("Return"), searchEdit);
    connect(searchShortcut, &QShortcut::activated, this, &MainWindow::onSearchEnterPressed);
}

void MainWindow::onShowAnalogues()
{
    if (!currentProduct) {
        QMessageBox::information(this, "Информация", "Выберите лекарство для просмотра аналогов.");
        return;
    }

    // Создаем диалоговое окно для просмотра аналогов
    QDialog *analoguesDialog = new QDialog(this);
    analoguesDialog->setWindowTitle("Аналоги для: " + QString::fromStdString(currentProduct->getName()));
    analoguesDialog->setMinimumSize(800, 600);
    analoguesDialog->setStyleSheet(this->styleSheet()); // Применяем стиль

    QVBoxLayout *dialogLayout = new QVBoxLayout(analoguesDialog);

    // Список аналогов
    QListWidget *analoguesList = new QListWidget;

    try {
        if (auto medicine = std::dynamic_pointer_cast<Medicine>(currentProduct)) {
            auto analogues = medicine->getAnalogues();

            if (analogues.empty()) {
                QListWidgetItem *noItem = new QListWidgetItem("Аналоги не найдены");
                noItem->setFlags(noItem->flags() & ~Qt::ItemIsEnabled); // Делаем неактивным
                analoguesList->addItem(noItem);
            } else {
                for (const auto& analogue : analogues) {
                    QString displayText = QString("%1 (%2) - %3 руб.")
                                              .arg(QString::fromStdString(analogue->getName()))
                                              .arg(QString::fromStdString(analogue->getId()))
                                              .arg(analogue->getBasePrice(), 0, 'f', 2);

                    QListWidgetItem *item = new QListWidgetItem(displayText, analoguesList);
                    item->setData(Qt::UserRole, QString::fromStdString(analogue->getId()));
                }
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка загрузки аналогов: %1").arg(e.what()));
    }

    // Текст для детальной информации
    QTextEdit *detailsText = new QTextEdit;
    detailsText->setReadOnly(true);
    detailsText->setPlainText("Выберите аналог для просмотра детальной информации");

    // Разделяем окно на список и детали
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(analoguesList);
    splitter->addWidget(detailsText);
    splitter->setSizes(QList<int>() << 300 << 500);

    // Кнопки
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Close);

    dialogLayout->addWidget(new QLabel("Список аналогов:"));
    dialogLayout->addWidget(splitter, 1);
    dialogLayout->addWidget(buttonBox);

    // Подключаем сигналы
    connect(buttonBox, &QDialogButtonBox::rejected, analoguesDialog, &QDialog::reject);
    connect(analoguesList, &QListWidget::itemClicked, [this, detailsText](QListWidgetItem *item) {
        if (!item) return;

        QString productId = item->data(Qt::UserRole).toString();
        if (!productId.isEmpty()) {
            showProductDetailsInDialog(productId, detailsText);
        }
    });

    analoguesDialog->exec();
    delete analoguesDialog;
}

void MainWindow::showProductDetailsInDialog(const QString& productId, QTextEdit* textEdit)
{
    try {
        auto allProducts = pharmacyManager.getAllProducts();
        auto it = std::find_if(allProducts.begin(), allProducts.end(),
                               [&productId](const std::shared_ptr<MedicalProduct>& product) {
                                   return product->getId() == productId.toStdString();
                               });

        if (it != allProducts.end()) {
            auto product = *it;
            QString details;

            // Заголовок - цветной
            details += QString("<h1 style='color: #2E7D32;'>%1</h1>").arg(QString::fromStdString(product->getName()));

            // Основная информация
            details += "<h3 style='color: #2E7D32;'>Основная информация</h3>";
            details += QString("<p><b>ID:</b> %1</p>").arg(QString::fromStdString(product->getId()));
            details += QString("<p><b>Цена:</b> %1 руб.</p>").arg(product->getBasePrice(), 0, 'f', 2);
            details += QString("<p><b>Страна:</b> %1</p>").arg(QString::fromStdString(product->getManufacturerCountry()));
            details += QString("<p><b>Срок годности:</b> %1</p>").arg(QString::fromStdString(product->getExpirationDate()));

            details += "<hr>";

            // Информация о лекарстве (если это Medicine)
            if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
                details += "<h3 style='color: #2E7D32;'>Лекарственная информация</h3>";

                details += QString("<p><b>Активное вещество:</b> %1</p>").arg(QString::fromStdString(medicine->getActiveSubstance()));
                details += QString("<p><b>Рецептурный:</b> %1</p>").arg(medicine->getIsPrescription() ? "Да" : "Нет");
                details += QString("<p><b>Инструкция:</b> %1</p>").arg(QString::fromStdString(medicine->getInstructions()));
                details += QString("<p><b>Форма выпуска:</b> %1</p>").arg(QString::fromStdString(medicine->getDosageForm()));
                details += QString("<p><b>Способ применения:</b> %1</p>").arg(QString::fromStdString(medicine->getAdministrationMethod()));

                // Дополнительная информация для сиропа
                if (medicine->getDosageForm() == "Syrup") {
                    details += "<hr>";
                    details += "<h3 style='color: #2E7D32;'>Характеристики сиропа</h3>";

                    details += QString("<p><b>Объем:</b> %1</p>").arg("100 мл"); // Замени на актуальные данные
                    details += QString("<p><b>Содержит сахар:</b> %1</p>").arg("Нет"); // Замени на актуальные данные
                    details += QString("<p><b>Вкус:</b> %1</p>").arg("ауц"); // Замени на актуальные данные
                }
            }

            textEdit->setHtml(details);
        } else {
            textEdit->setPlainText("Продукт не найден");
        }
    } catch (const std::exception& e) {
        textEdit->setPlainText(QString("Ошибка загрузки данных: %1").arg(e.what()));
    }
}


void MainWindow::onAddAnalogue()
{
    if (!currentProduct) {
        QMessageBox::information(this, "Информация",
                                 "Сначала выберите лекарство из списка.");
        return;
    }

    // Приводим к Medicine
    auto currentMedicine = std::dynamic_pointer_cast<Medicine>(currentProduct);
    if (!currentMedicine) {
        QMessageBox::warning(this, "Ошибка",
                             "Выбранный продукт не является лекарством.");
        return;
    }

    // Получаем все лекарства для выбора аналогов
    std::vector<std::shared_ptr<Medicine>> allMedicines;
    auto allProducts = pharmacyManager.getAllProducts();
    for (const auto& product : allProducts) {
        if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
            allMedicines.push_back(medicine);
        }
    }

    // Создаем и показываем диалог
    AnaloguesDialog dialog(currentMedicine, allMedicines, this);
    if (dialog.exec() == QDialog::Accepted) {
        try {
            // Получаем выбранные аналоги
            auto selectedAnalogueIds = dialog.getSelectedAnalogues();

            // Очищаем текущие аналоги
            currentMedicine->clearAnalogues();

            // Удаляем старые аналоги из файла
            auto oldAnalogues = FileManager::getInstance().getMedicineAnalogues(currentMedicine->getId());
            for (const auto& oldAnalogueId : oldAnalogues) {
                FileManager::getInstance().removeAnalogue(currentMedicine->getId(), oldAnalogueId);
            }

            // Добавляем новые аналоги
            for (const auto& analogueId : selectedAnalogueIds) {
                // Находим лекарство по ID
                auto it = std::find_if(allMedicines.begin(), allMedicines.end(),
                                       [&analogueId](const std::shared_ptr<Medicine>& med) {
                                           return med->getId() == analogueId;
                                       });

                if (it != allMedicines.end()) {
                    currentMedicine->addAnalogue(*it);
                    // Сохраняем в файл аналогов
                    if (!FileManager::getInstance().addAnalogue(currentMedicine->getId(), analogueId)) {
                        QMessageBox::warning(this, "Предупреждение",
                                             "Не удалось сохранить аналог в файл: " + QString::fromStdString(analogueId));
                    }
                }
            }

            QMessageBox::information(this, "Успех",
                                     "Аналоги успешно добавлены!");

        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Ошибка",
                                  QString("Ошибка при добавлении аналогов: %1").arg(e.what()));
        }
    }
}

void MainWindow::setupColors()
{
    setStyleSheet(
        "QMainWindow { background-color: #f5f5f5; }"
        "QGroupBox {"
        "    font-weight: bold;"
        "    border: 2px solid #4CAF50;"
        "    border-radius: 8px;"
        "    margin-top: 1ex;"
        "    padding: 15px;"
        "    background-color: #f8fff8;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    subcontrol-position: top center;"
        "    padding: 0 10px;"
        "    background-color: #f8fff8;"
        "    color: #2E7D32;"
        "}"
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    border: 2px solid #45a049;"
        "    border-radius: 6px;"
        "    padding: 10px;"
        "    font-weight: bold;"
        "    color: white;"
        "    min-height: 20px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
        "QLineEdit {"
        "    border: 2px solid #4CAF50;"
        "    border-radius: 6px;"
        "    padding: 8px;"
        "    font-size: 14px;"
        "    background-color: white;"
        "}"
        "QListWidget {"
        "    border: 2px solid #4CAF50;"
        "    border-radius: 6px;"
        "    background-color: white;"
        "    font-size: 14px;"
        "    alternate-background-color: #f0fff0;"
        "}"
        "QListWidget::item {"
        "    padding: 12px;"
        "    border-bottom: 1px solid #e0e0e0;"
        "}"
        "QListWidget::item:hover {"
        "    background-color: #e8f5e8;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "}"
        "QTextEdit {"
        "    border: 2px solid #4CAF50;"
        "    border-radius: 6px;"
        "    background-color: white;"
        "    font-size: 14px;"
        "    padding: 15px;"
        "}"
        "QLabel {"
        "    color: #2E7D32;"
        "    font-weight: bold;"
        "}"
        );
}


void MainWindow::loadAllData()
{
    try {

        // Загружаем лекарства из файла
        std::vector<std::shared_ptr<Medicine>> medicines;
        if (FileManager::getInstance().loadMedicines(medicines)) {
            for (const auto& medicine : medicines) {
                pharmacyManager.addProduct(medicine);
            }
            qDebug() << "Загружено лекарств:" << medicines.size();
        } else {
            QMessageBox::warning(this, "Предупреждение",
                                 "Не удалось загрузить данные о лекарствах. Файл может быть пустым или отсутствовать.");
        }

        // Загружаем аптеки из файла
        std::vector<Pharmacy> pharmacies;
        if (FileManager::getInstance().loadPharmacies(pharmacies)) {
            for (const auto& pharmacy : pharmacies) {
                pharmacyManager.addPharmacy(std::make_shared<Pharmacy>(pharmacy));
            }
            qDebug() << "Загружено аптек:" << pharmacies.size();
        }

        // Загружаем инвентарные операции из файла
        std::vector<std::shared_ptr<InventoryOperation>> operations;
        if (FileManager::getInstance().loadInventoryOperations(operations)) {
            for (const auto& operation : operations) {
               pharmacyManager.addOperation(operation);
            }
            qDebug() << "Загружено операций:" << operations.size();
        }

        // Загружаем данные о запасах
        if (FileManager::getInstance().loadStockData(pharmacies, medicines)) {
            qDebug() << "Данные о запасах загружены";
        }

        updateCompleter();
        showProductList();

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Критическая ошибка при загрузке данных: %1").arg(e.what()));
    }
}


void MainWindow::showProductList()
{
    if (!productsList || !contentText) return;

    // Показываем список, скрываем текстовое поле
    productsList->setVisible(true);
    contentText->setVisible(false);

    productsList->clear();

    try {
        // ИСПОЛЬЗУЕМ РЕАЛЬНЫЕ ДАННЫЕ ИЗ PHARMACYMANAGER
        auto allProducts = pharmacyManager.getAllProducts();

        if (allProducts.empty()) {
            productsList->setVisible(false);
            contentText->setVisible(true);
            contentText->setPlainText("Нет доступных лекарств. Добавьте новое лекарство.");
            return;
        }

        for (const auto& product : allProducts) {
            QString displayText = QString("%1 (%2) - %3 руб.")
                                      .arg(QString::fromStdString(product->getName()))
                                      .arg(QString::fromStdString(product->getId()))
                                      .arg(product->getBasePrice(), 0, 'f', 2);

            QListWidgetItem *item = new QListWidgetItem(displayText, productsList);
            item->setData(Qt::UserRole, QString::fromStdString(product->getId()));
        }

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка загрузки списка: %1").arg(e.what()));

        // Fallback: показать сообщение об ошибке
        productsList->setVisible(false);
        contentText->setVisible(true);
        contentText->setPlainText("Ошибка загрузки данных: " + QString(e.what()));
    }
}

void MainWindow::showProductDetails(const QString& productId, const QString& productName)
{
    Q_UNUSED(productName); // Не используем, но оставляем для совместимости

    productsList->setVisible(false);
    contentText->setVisible(true);

    try {
        auto product = pharmacyManager.getProduct(productId.toStdString());
        if (product) {
            QString details = formatProductDetails(product);
            contentText->setHtml(details);
        } else {
            contentText->setPlainText("Лекарство не найдено.");
        }
    } catch (const std::exception& e) {
        contentText->setPlainText(QString("Ошибка загрузки данных: %1").arg(e.what()));
    }
}

QString MainWindow::formatProductDetails(std::shared_ptr<MedicalProduct> product)
{
    QString details;

    // Базовая информация
    details += "<div style='font-family: Arial, sans-serif;'>";
    details += "<h1 style='color: #2E7D32;'>" + QString::fromStdString(product->getName()) + "</h1>";
    details += "<div style='background: #f5f5f5; padding: 15px; border-radius: 5px;'>";
    details += "<h3 style='color: #1976D2;'>Основная информация</h3>";
    details += "<table style='width: 100%;'>";
    details += "<tr><td style='width: 200px; padding: 5px;'><b>ID:</b></td><td>" + QString::fromStdString(product->getId()) + "</td></tr>";
    details += "<tr><td style='padding: 5px;'><b>Цена:</b></td><td>" + QString::number(product->getBasePrice(), 'f', 2) + " руб.</td></tr>";
    details += "<tr><td style='padding: 5px;'><b>Страна:</b></td><td>" + QString::fromStdString(product->getManufacturerCountry()) + "</td></tr>";
    details += "<tr><td style='padding: 5px;'><b>Срок годности:</b></td><td>" + QString::fromStdString(product->getExpirationDate()) + "</td></tr>"; // Убрано .toString()
    details += "</table>";
    details += "</div>";

    // Информация о лекарстве (если это Medicine)
    if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
        details += "<div style='background: #E3F2FD; padding: 15px; border-radius: 5px; margin-top: 10px;'>";
        details += "<h3 style='color: #1976D2;'>Лекарственная информация</h3>";
        details += "<table style='width: 100%;'>";
        details += "<tr><td style='width: 200px; padding: 5px;'><b>Активное вещество:</b></td><td>" + QString::fromStdString(medicine->getActiveSubstance()) + "</td></tr>";
        details += "<tr><td style='padding: 5px;'><b>Рецептурный:</b></td><td>" + QString(medicine->getIsPrescription() ? "Да" : "Нет") + "</td></tr>"; // Добавлен QString()
        details += "<tr><td style='padding: 5px;'><b>Инструкция:</b></td><td>" + QString::fromStdString(medicine->getInstructions()) + "</td></tr>";
        details += "<tr><td style='padding: 5px;'><b>Форма выпуска:</b></td><td>" + QString::fromStdString(medicine->getDosageForm()) + "</td></tr>";
        details += "<tr><td style='padding: 5px;'><b>Способ применения:</b></td><td>" + QString::fromStdString(medicine->getAdministrationMethod()) + "</td></tr>";
        details += "</table>";
        details += "</div>";
    }

    // Специфичная информация для разных типов
    if (auto tablet = std::dynamic_pointer_cast<Tablet>(product)) {
        details += "<div style='background: #FFF3E0; padding: 15px; border-radius: 5px; margin-top: 10px;'>";
        details += "<h3 style='color: #EF6C00;'>Характеристики таблеток</h3>";
        details += "<table style='width: 100%;'>";
        details += "<tr><td style='width: 200px; padding: 5px;'><b>Таблеток в упаковке:</b></td><td>" + QString::number(tablet->getUnitsPerPackage()) + "</td></tr>";
        details += "<tr><td style='padding: 5px;'><b>Дозировка:</b></td><td>" + QString::number(tablet->getDosageMg()) + " мг</td></tr>"; // Исправлено getDosageMe() на getDosageMg()
        details += "<tr><td style='padding: 5px;'><b>Оболочка:</b></td><td>" + QString::fromStdString(tablet->getCoating()) + "</td></tr>";
        details += "</table>";
        details += "</div>";
    }
    else if (auto syrup = std::dynamic_pointer_cast<Syrup>(product)) {
        details += "<div style='background: #FFF3E0; padding: 15px; border-radius: 5px; margin-top: 10px;'>";
        details += "<h3 style='color: #EF6C00;'>Характеристики сиропа</h3>";
        details += "<table style='width: 100%;'>";
        details += "<tr><td style='width: 200px; padding: 5px;'><b>Объем:</b></td><td>" + QString::number(syrup->getVolumeMl()) + " мл</td></tr>"; // Исправлено getVolumeM1() на getVolumeMl()
        details += "<tr><td style='padding: 5px;'><b>Содержит сахар:</b></td><td>" + QString(syrup->getHasSugar() ? "Да" : "Нет") + "</td></tr>"; // Добавлен QString()
        details += "<tr><td style='padding: 5px;'><b>Вкус:</b></td><td>" + QString::fromStdString(syrup->getFlavor()) + "</td></tr>";
        details += "</table>";
        details += "</div>";
    }
    else if (auto ointment = std::dynamic_pointer_cast<Ointment>(product)) {
        details += "<div style='background: #FFF3E0; padding: 15px; border-radius: 5px; margin-top: 10px;'>";
        details += "<h3 style='color: #EF6C00;'>Характеристики мази</h3>";
        details += "<table style='width: 100%;'>";
        details += "<tr><td style='width: 200px; padding: 5px;'><b>Вес:</b></td><td>" + QString::number(ointment->getWeightG()) + " г</td></tr>";
        details += "<tr><td style='padding: 5px;'><b>Тип основы:</b></td><td>" + QString::fromStdString(ointment->getBaseType()) + "</td></tr>";
        details += "</table>";
        details += "</div>";
    }

    details += "</div>";
    return details;
}

void MainWindow::showEditPanel()
{
    if (editPanel && editPanel->isHidden()) {
        editPanel->show();
        isEditMode = true;
    }
}

void MainWindow::hideEditPanel()
{
    if (editPanel && !editPanel->isHidden()) {
        editPanel->hide();
        isEditMode = false;
    }
}

void MainWindow::updateCompleter()
{
    QStringList wordList;

    try {
        auto allProducts = pharmacyManager.getAllProducts();
        for (const auto& product : allProducts) {
            wordList << QString::fromStdString(product->getName());
            wordList << QString::fromStdString(product->getId());

            if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
                wordList << QString::fromStdString(medicine->getActiveSubstance());
            }
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка обновления автодополнения: %1").arg(e.what()));
    }

    searchCompleter->setModel(new QStringListModel(wordList, searchCompleter));
}

// ===== SLOTS =====

void MainWindow::onSearchTextChanged(const QString &text)
{
    // Автопоиск при вводе (опционально)
    if (text.length() >= 2) {
        performSearch(text);
    } else if (text.isEmpty()) {
        onShowAllProducts();
    }
}

void MainWindow::onSearchButtonClicked()
{
    QString searchText = searchEdit->text().trimmed();
    if (searchText.isEmpty()) {
        onShowAllProducts();
        return;
    }

    performSearch(searchText);
}

void MainWindow::onSearchEnterPressed()
{
    onSearchButtonClicked();
}

void MainWindow::onShowAllProducts()
{
    productsList->setVisible(true);
    contentText->setVisible(false);
    productsList->clear();

    try {
        auto allProducts = pharmacyManager.getAllProducts();
        for (const auto& product : allProducts) {
            QString displayText = QString("%1 (%2) - %3 руб.")
                                      .arg(QString::fromStdString(product->getName()))
                                      .arg(QString::fromStdString(product->getId()))
                                      .arg(product->getBasePrice(), 0, 'f', 2);

            QListWidgetItem *item = new QListWidgetItem(displayText, productsList);
            item->setData(Qt::UserRole, QString::fromStdString(product->getId()));
        }
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка загрузки списка: %1").arg(e.what()));
    }
}

void MainWindow::onProductSelected(QListWidgetItem *item)
{
    if (!item) return;

    try {
        // Получаем ID из элемента списка
        QString productId = item->data(Qt::UserRole).toString();

        // Ищем продукт по ID в менеджере
        auto allProducts = pharmacyManager.getAllProducts();
        auto it = std::find_if(allProducts.begin(), allProducts.end(),
                               [&productId](const std::shared_ptr<MedicalProduct>& product) {
                                   return product->getId() == productId.toStdString();
                               });

        if (it != allProducts.end()) {
            currentProduct = *it;
            // ИСПРАВЛЕННЫЙ ВЫЗОВ - передаем оба аргумента
            showProductDetails(productId, QString::fromStdString(currentProduct->getName()));
        } else {
            qDebug() << "Продукт с ID" << productId << "не найден";
        }

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Ошибка при выборе продукта: %1").arg(e.what()));
    }
}


void MainWindow::onShowSupplies()
{
    SuppliesDialog dialog(this);
    dialog.exec();
}

void MainWindow::onShowReturns()
{
    ReturnsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onShowWriteOffs()
{
    WriteOffsDialog dialog(this);
    dialog.exec();
}


// mainwindow.cpp - метод onShowPharmacyAvailability
void MainWindow::onShowPharmacyAvailability()
{
    // Проверяем, выбрано ли лекарство
    if (!currentProduct) {
        QMessageBox::information(this, "Информация",
                                 "Сначала выберите лекарство из списка.");
        return;
    }

    // Проверяем, является ли это лекарством (а не другим MedicalProduct)
    auto medicine = std::dynamic_pointer_cast<Medicine>(currentProduct);
    if (!medicine) {
        QMessageBox::information(this, "Информация",
                                 "Выбранный продукт не является лекарством.");
        return;
    }

    // Создаем и показываем диалог
    SimpleAvailabilityDialog dialog(
        medicine->getId(),      // ID лекарства (только цифры)
        medicine->getName(),    // Название лекарства
        this
        );

    dialog.exec();
}

void MainWindow::onAddProduct()
{
    AddProductDialog *dialog = new AddProductDialog(this);
    dialog->setEditMode(false);

    if (dialog->exec() == QDialog::Accepted) {
        try {
            // Вся валидация теперь происходит в конструкторах лекарств
            // при создании объектов через make_shared

            QString type = dialog->getType();
            QString id = dialog->getId().trimmed();
            QString name = dialog->getName().trimmed();
            double price = dialog->getPrice();
            QDate expiryDate = dialog->getExpiryDate();
            QString country = dialog->getCountry().trimmed();
            bool prescription = dialog->isPrescription();
            QString substance = dialog->getSubstance().trimmed();
            QString instructions = dialog->getInstructions().trimmed();

            // Проверка уникальности ID
            try {
                pharmacyManager.getProduct(id.toStdString());
                throw InvalidProductDataException("ID", "уже существует");
            } catch (...) {
                // ID свободен - продолжаем
            }

            // Проверка даты
            if (!expiryDate.isValid() || expiryDate <= QDate::currentDate()) {
                throw InvalidProductDataException("Срок годности", "должен быть в будущем");
            }

            SafeDate safeDate(expiryDate.year(), expiryDate.month(), expiryDate.day());
            std::shared_ptr<MedicalProduct> newProduct;

            // Создание объекта - валидация произойдет в конструкторе
            if (type == "Таблетки") {
                newProduct = std::make_shared<Tablet>(
                    id.toStdString(), name.toStdString(), price, safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getTabletUnits(), dialog->getTabletDosage(),
                    dialog->getTabletCoating().trimmed().toStdString()
                    );
            }
            else if (type == "Сироп") {
                newProduct = std::make_shared<Syrup>(
                    id.toStdString(), name.toStdString(), price, safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getSyrupVolume(), dialog->getSyrupHasSugar(),
                    dialog->getSyrupFlavor().trimmed().toStdString()
                    );
            }
            else if (type == "Мазь") {
                newProduct = std::make_shared<Ointment>(
                    id.toStdString(), name.toStdString(), price, safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getOintmentWeight(),
                    dialog->getOintmentBase().trimmed().toStdString()
                    );
            }

            // Добавление и сохранение
            if (newProduct) {
                pharmacyManager.addProduct(newProduct);

                if (auto medicine = std::dynamic_pointer_cast<Medicine>(newProduct)) {
                    if (!FileManager::getInstance().addMedicine(medicine)) {
                        pharmacyManager.removeProduct(id.toStdString());
                        throw std::runtime_error("Ошибка записи в файл");
                    }
                }

                updateCompleter();
                onShowAllProducts();
                QMessageBox::information(this, "Успех", "Лекарство успешно добавлено!");
            }

        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", QString("Ошибка добавления: %1").arg(e.what()));
        }
    }

    delete dialog;
}

void MainWindow::onEditProduct()
{
    QListWidgetItem *item = productsList->currentItem();
    if (!item) {
        QMessageBox::information(this, "Информация", "Выберите лекарство для редактирования.");
        return;
    }

    QString productId = item->data(Qt::UserRole).toString();
    saveProductChanges(productId);
}

void MainWindow::saveProductChanges(const QString& productId)
{
    std::shared_ptr<MedicalProduct> originalProduct;
    AddProductDialog *dialog = nullptr;

    try {
        // Получаем текущий продукт
        originalProduct = pharmacyManager.getProduct(productId.toStdString());
        if (!originalProduct) {
            QMessageBox::warning(this, "Ошибка", "Продукт не найден");
            return;
        }

        // Создаем диалог редактирования
        dialog = new AddProductDialog(this);
        dialog->setEditMode(true);

        // Заполняем диалог текущими данными продукта
        if (auto medicine = std::dynamic_pointer_cast<Medicine>(originalProduct)) {
            // Преобразуем строку с датой в QDate
            std::string expDateStr = originalProduct->getExpirationDate();
            QDate expiryDate = QDate::fromString(QString::fromStdString(expDateStr), "yyyy-MM-dd");

            // Если парсинг не удался, используем текущую дату + 2 года
            if (!expiryDate.isValid()) {
                expiryDate = QDate::currentDate().addYears(2);
            }

            // ОБЩИЕ ДАННЫЕ ДЛЯ ВСЕХ ТИПОВ
            QString id = QString::fromStdString(originalProduct->getId());
            QString name = QString::fromStdString(originalProduct->getName());
            double price = originalProduct->getBasePrice();
            QString country = QString::fromStdString(originalProduct->getManufacturerCountry());
            bool prescription = medicine->getIsPrescription();
            QString substance = QString::fromStdString(medicine->getActiveSubstance());
            QString instructions = QString::fromStdString(medicine->getInstructions());

            // Заполняем данные в зависимости от типа лекарства
            if (auto tablet = std::dynamic_pointer_cast<Tablet>(originalProduct)) {
                dialog->setProductData(
                    id, name, price, "Таблетки", expiryDate, country,
                    prescription, substance, instructions,
                    tablet->getUnitsPerPackage(),
                    tablet->getDosageMg(),
                    QString::fromStdString(tablet->getCoating())
                    );
            }
            else if (auto syrup = std::dynamic_pointer_cast<Syrup>(originalProduct)) {
                dialog->setProductData(
                    id, name, price, "Сироп", expiryDate, country,
                    prescription, substance, instructions,
                    syrup->getVolumeMl(),
                    syrup->getHasSugar(),
                    QString::fromStdString(syrup->getFlavor())
                    );
            }
            else if (auto ointment = std::dynamic_pointer_cast<Ointment>(originalProduct)) {
                dialog->setProductData(
                    id, name, price, "Мазь", expiryDate, country,
                    prescription, substance, instructions,
                    ointment->getWeightG(),
                    QString::fromStdString(ointment->getBaseType())
                    );
            }
            else {
                // Резервный вариант - заполняем только основные поля
                dialog->setProductData(
                    id, name, price, "Таблетки", expiryDate, country,
                    prescription, substance, instructions
                    );
            }
        }

        // Показываем диалог и ждем результат
        if (dialog->exec() == QDialog::Accepted) {
            // Получаем обновленные данные из диалога
            QString type = dialog->getType();
            QString id = dialog->getId().trimmed();
            QString name = dialog->getName().trimmed();
            double price = dialog->getPrice();
            QDate expiryDate = dialog->getExpiryDate();
            QString country = dialog->getCountry().trimmed();
            bool prescription = dialog->isPrescription();
            QString substance = dialog->getSubstance().trimmed();
            QString instructions = dialog->getInstructions().trimmed();

            // Валидация данных
            if (id.isEmpty()) throw InvalidProductDataException("ID", "не может быть пустым");
            if (name.isEmpty()) throw InvalidProductDataException("Название", "не может быть пустым");
            if (country.isEmpty()) throw InvalidProductDataException("Страна", "не может быть пустой");
            if (substance.isEmpty()) throw InvalidProductDataException("Активное вещество", "не может быть пустым");
            if (instructions.isEmpty()) throw InvalidProductDataException("Инструкция", "не может быть пустой");

            if (id.length() < 3) throw InvalidProductDataException("ID", "должен содержать минимум 3 цифры");

            // Проверка что ID состоит только из цифр
            bool isNumeric = true;
            for (QChar c : id) {
                if (!c.isDigit()) {
                    isNumeric = false;
                    break;
                }
            }
            if (!isNumeric) throw InvalidProductDataException("ID", "должен содержать только цифры");

            if (price <= 0) throw InvalidProductDataException("Цена", "должна быть положительной");
            if (!expiryDate.isValid()) throw InvalidProductDataException("Срок годности", "неверная дата");
            if (expiryDate <= QDate::currentDate()) throw InvalidProductDataException("Срок годности", "должен быть в будущем");

            // Если ID изменился, проверяем на уникальность
            if (id != productId) {
                try {
                    pharmacyManager.getProduct(id.toStdString());
                    throw InvalidProductDataException("ID", "уже существует");
                } catch (...) {
                    // ID свободен - продолжаем
                }
            }

            SafeDate safeDate(expiryDate.year(), expiryDate.month(), expiryDate.day());
            std::shared_ptr<MedicalProduct> updatedProduct;

            // Создаем обновленный продукт
            if (type == "Таблетки") {
                QString coating = dialog->getTabletCoating().trimmed();
                if (coating.isEmpty()) throw InvalidProductDataException("Покрытие", "не может быть пустым");

                updatedProduct = std::make_shared<Tablet>(
                    id.toStdString(), name.toStdString(), price, safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getTabletUnits(), dialog->getTabletDosage(), coating.toStdString()
                    );
            }
            else if (type == "Сироп") {
                QString flavor = dialog->getSyrupFlavor().trimmed();
                if (flavor.isEmpty()) throw InvalidProductDataException("Вкус", "не может быть пустым");

                updatedProduct = std::make_shared<Syrup>(
                    id.toStdString(), name.toStdString(), price, safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getSyrupVolume(), dialog->getSyrupHasSugar(), flavor.toStdString()
                    );
            }
            else if (type == "Мазь") {
                QString base = dialog->getOintmentBase().trimmed();
                if (base.isEmpty()) throw InvalidProductDataException("Основа", "не может быть пустой");

                updatedProduct = std::make_shared<Ointment>(
                    id.toStdString(), name.toStdString(), price, safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getOintmentWeight(), base.toStdString()
                    );
            }

            // Сохраняем изменения
            if (updatedProduct) {
                auto backupProduct = originalProduct;
                pharmacyManager.updateProduct(updatedProduct);

                // Перезаписываем файл
                auto allProducts = pharmacyManager.getAllProducts();
                std::vector<std::shared_ptr<Medicine>> medicines;
                for (const auto& product : allProducts) {
                    if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
                        medicines.push_back(medicine);
                    }
                }

                if (!FileManager::getInstance().saveMedicines(medicines)) {
                    pharmacyManager.updateProduct(backupProduct);
                    throw std::runtime_error("Ошибка записи в файл");
                }

                // Обновляем интерфейс
                showProductDetails(id, QString::fromStdString(updatedProduct->getName()));
                onShowAllProducts();
                QMessageBox::information(this, "Успех", "Изменения сохранены!");
            }
        }

    }
    catch (const InvalidProductDataException& e) {
        QMessageBox::warning(this, "Ошибка валидации",
                             QString("Некорректные данные:\n%1").arg(e.what()));
    }
    catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Ошибка при сохранении изменений: %1").arg(e.what()));
    }

    if (dialog) {
        delete dialog;
    }
}

void MainWindow::onDeleteProduct()
{
    QListWidgetItem *item = productsList->currentItem();
    if (!item) {
        QMessageBox::information(this, "Информация", "Выберите лекарство для удаления.");
        return;
    }

    QString productId = item->data(Qt::UserRole).toString();
    QString productName = item->text().left(item->text().indexOf(" ("));

    int ret = QMessageBox::question(this, "Удаление",
                                    "Вы уверены, что хотите удалить:\n" + productName + "?",
                                    QMessageBox::Yes | QMessageBox::No);
    if (ret == QMessageBox::Yes) {
        try {
            // Удаляем из менеджера
            pharmacyManager.removeProduct(productId.toStdString());

            // ПЕРЕЗАПИСЫВАЕМ ВЕСЬ ФАЙЛ БЕЗ УДАЛЕННОГО ПРОДУКТА
            auto allProducts = pharmacyManager.getAllProducts();
            std::vector<std::shared_ptr<Medicine>> medicines;

            // Фильтруем только Medicine объекты для записи в файл
            for (const auto& product : allProducts) {
                if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
                    medicines.push_back(medicine);
                }
            }

            // Сохраняем обновленный список в файл используя перегруженные операторы
            if (!FileManager::getInstance().saveMedicines(medicines)) {
                QMessageBox::warning(this, "Ошибка",
                                     "Лекарство удалено, но не сохранено в файл!");
            } else {
                qDebug() << "Файл лекарств перезаписан после удаления";
            }

            // Обновляем интерфейс
            delete productsList->takeItem(productsList->row(item));
            updateCompleter();

            contentText->setVisible(true);
            contentText->setPlainText("Выберите лекарство из списка для просмотра информации.");

            QMessageBox::information(this, "Успех", "Лекарство успешно удалено!");

        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка",
                                 QString("Ошибка удаления: %1").arg(e.what()));
        }
    }
}



void MainWindow::onSaveChanges()
{
    pushToUndoStack();
    QMessageBox::information(this, "Сохранение", "Изменения сохранены (заглушка)");
}

void MainWindow::onCancelEdit()
{
    hideEditPanel();
    QMessageBox::information(this, "Отмена", "Редактирование отменено");
}

void MainWindow::onUndo()
{
    if (!undoStack.isEmpty()) {
        undoStack.pop();
        QMessageBox::information(this, "Отмена", "Последнее действие отменено (заглушка)");
    } else {
        QMessageBox::information(this, "Отмена", "Нет действий для отмены");
    }
}

void MainWindow::pushToUndoStack()
{
    undoStack.push("current_state");
    if (undoStack.size() > 10) {
        undoStack.remove(0);
    }
}

void MainWindow::onItemSelected()
{
    // Проверяем, есть ли выбранные элементы
    if (productsList->selectedItems().count() > 0)
        showEditPanel();
    else
        hideEditPanel();
}

void MainWindow::performSearch(const QString &searchText)
{
    if (searchText.isEmpty()) {
        onShowAllProducts();
        return;
    }

    productsList->setVisible(true);
    contentText->setVisible(false);
    productsList->clear();

    try {
        auto searchResults = pharmacyManager.searchProducts(searchText.toStdString());
        bool found = false;

        for (const auto& product : searchResults) {
            QString displayText = QString("%1 (%2) - %3 руб.")
                                      .arg(QString::fromStdString(product->getName()))
                                      .arg(QString::fromStdString(product->getId()))
                                      .arg(product->getBasePrice(), 0, 'f', 2);

            QListWidgetItem *item = new QListWidgetItem(displayText, productsList);
            item->setData(Qt::UserRole, QString::fromStdString(product->getId()));
            found = true;
        }

        if (!found) {
            productsList->setVisible(false);
            contentText->setVisible(true);
            contentText->setHtml(
                "<div style='text-align: center; padding: 50px; color: #666;'>"
                "<h3 style='color: #F44336;'>Ничего не найдено</h3>"
                "<p>По запросу: <b>\"" + searchText + "\"</b></p>"
                               "<p>Попробуйте изменить запрос или посмотрите все лекарства</p>"
                               "</div>"
                );
        }

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка поиска: %1").arg(e.what()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    try {
        // Сохраняем все данные при закрытии приложения
        auto allProducts = pharmacyManager.getAllProducts();
        std::vector<std::shared_ptr<Medicine>> medicines;

        for (const auto& product : allProducts) {
            if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
                medicines.push_back(medicine);
            }
        }

        if (!FileManager::getInstance().saveMedicines(medicines)) {
            QMessageBox::warning(this, "Ошибка",
                                 "Не удалось сохранить данные о лекарствах при закрытии приложения!");
        }


        qDebug() << "Все данные сохранены, приложение закрывается";

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Ошибка",
                              QString("Ошибка при сохранении данных: %1").arg(e.what()));
    }

    event->accept();
}
