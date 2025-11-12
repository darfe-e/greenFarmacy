#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
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

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , searchEdit(nullptr)
    , productsList(nullptr)
    , contentText(nullptr)
    , searchCompleter(new QCompleter(this))
    , isEditMode(false)
{
    setupUI();
    setupColors();
    loadSampleData(); // Загружаем тестовые данные
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

    // Навигация
    QGroupBox *navGroup = new QGroupBox("Навигация");
    QVBoxLayout *navLayout = new QVBoxLayout(navGroup);

    suppliesBtn = new QPushButton("Поставки");
    returnsBtn = new QPushButton("Возвраты");
    writeOffsBtn = new QPushButton("Списания");
    analoguesBtn = new QPushButton("Аналоги");
    availabilityBtn = new QPushButton("Наличие в аптеках");
    addProductBtn = new QPushButton("Добавить лекарство");

    navLayout->addWidget(suppliesBtn);
    navLayout->addWidget(returnsBtn);
    navLayout->addWidget(writeOffsBtn);
    navLayout->addWidget(analoguesBtn);
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
    cancelEditBtn = new QPushButton("Отменить");

    editLayout->addWidget(editLabel);
    editLayout->addWidget(editBtn);
    editLayout->addWidget(deleteBtn);
    editLayout->addWidget(cancelEditBtn);
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
    connect(analoguesBtn, &QPushButton::clicked, this, &MainWindow::onShowAnalogues);
    connect(availabilityBtn, &QPushButton::clicked, this, &MainWindow::onShowPharmacyAvailability);
    connect(addProductBtn, &QPushButton::clicked, this, &MainWindow::onAddProduct);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveChanges);
    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::onUndo);
    connect(productsList, &QListWidget::itemClicked, this, &MainWindow::onProductSelected);
    connect(editBtn, &QPushButton::clicked, this, &MainWindow::onEditProduct);
    connect(deleteBtn, &QPushButton::clicked, this, &MainWindow::onDeleteProduct);
    connect(cancelEditBtn, &QPushButton::clicked, this, &MainWindow::onCancelEdit);
    connect(productsList, &QListWidget::itemSelectionChanged, this, &MainWindow::onItemSelected);

    // Горячие клавиши
    undoBtn->setShortcut(QKeySequence("Ctrl+Z"));
    QShortcut *searchShortcut = new QShortcut(QKeySequence("Return"), searchEdit);
    connect(searchShortcut, &QShortcut::activated, this, &MainWindow::onSearchEnterPressed);
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

void MainWindow::loadSampleData()
{
    try
    {
        // Создаем тестовые данные
        auto tablet1 = std::make_shared<Tablet>(
            "M001", "Парацетамол", 15.50,
            SafeDate(2026, 12, 31), "Россия",
            false, "Парацетамол", "По 1 таблетке 3 раза в день",
            20, 500.0, "пленочная"
            );

        auto tablet2 = std::make_shared<Tablet>(
            "M002", "Ибупрофен", 18.75,
            SafeDate(2026, 10, 15), "Россия",
            false, "Ибупрофен", "По 1 таблетке 2-3 раза в день",
            30, 400.0, "сахарная"
            );

        auto syrup1 = std::make_shared<Syrup>(
            "M003", "Амбробене", 45.30,
            SafeDate(2026, 8, 20), "Германия",
            false, "Амброксол", "По 5 мл 3 раза в день",
            100.0, true, "малиновый"
            );

        auto ointment1 = std::make_shared<Ointment>(
            "M004", "Троксевазин", 32.80,
            SafeDate(2026, 5, 10), "Болгария",
            false, "Троксерутин", "Наносить тонким слоем 2 раза в день",
            40.0, "гелевая"
            );

        // Добавляем в менеджер
        pharmacyManager.addProduct(tablet1);
        pharmacyManager.addProduct(tablet2);
        pharmacyManager.addProduct(syrup1);
        pharmacyManager.addProduct(ointment1);

    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "Ошибка", QString("Ошибка загрузки данных: %1").arg(e.what()));
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

    QString productId = item->data(Qt::UserRole).toString();
    QString productName = item->text().left(item->text().indexOf(" ("));

    showProductDetails(productId, productName);
    showEditPanel();
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

void MainWindow::onShowAnalogues()
{
    AnaloguesDialog dialog(this);
    dialog.exec();
}

void MainWindow::onShowPharmacyAvailability()
{
    AvailabilityDialog dialog(this);
    dialog.exec();
}

void MainWindow::onAddProduct()
{
    AddProductDialog *dialog = new AddProductDialog(this);
    dialog->setEditMode(false);

    if (dialog->exec() == QDialog::Accepted) {
        try {
            // Создаем новый продукт на основе данных из диалога
            std::shared_ptr<MedicalProduct> newProduct;

            QString type = dialog->getType();
            QString id = dialog->getId();
            QString name = dialog->getName();
            double price = dialog->getPrice();
            QDate expiryDate = dialog->getExpiryDate();
            QString country = dialog->getCountry();
            bool prescription = dialog->isPrescription();
            QString substance = dialog->getSubstance();
            QString instructions = dialog->getInstructions();

            SafeDate safeDate(expiryDate.year(), expiryDate.month(), expiryDate.day());

            if (type == "Таблетки") {
                newProduct = std::make_shared<Tablet>(
                    id.toStdString(), name.toStdString(), price,
                    safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getTabletUnits(),
                    dialog->getTabletDosage(),
                    dialog->getTabletCoating().toStdString()
                    );
            } else if (type == "Сироп") {
                newProduct = std::make_shared<Syrup>(
                    id.toStdString(), name.toStdString(), price,
                    safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getSyrupVolume(),
                    dialog->getSyrupHasSugar(),
                    dialog->getSyrupFlavor().toStdString()
                    );
            } else if (type == "Мазь") {
                newProduct = std::make_shared<Ointment>(
                    id.toStdString(), name.toStdString(), price,
                    safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getOintmentWeight(),
                    dialog->getOintmentBase().toStdString()
                    );
            }

            if (newProduct) {
                pharmacyManager.addProduct(newProduct);
                updateCompleter();
                onShowAllProducts();

                QMessageBox::information(this, "Успех", "Лекарство успешно добавлено!");
            }

        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", QString("Ошибка добавления лекарства: %1").arg(e.what()));
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
    try {
        // Получаем текущий продукт
        auto product = pharmacyManager.getProduct(productId.toStdString());
        if (!product) {
            QMessageBox::warning(this, "Ошибка", "Продукт не найден");
            return;
        }

        // Создаем диалог редактирования
        AddProductDialog *dialog = new AddProductDialog(this);
        dialog->setEditMode(true);

        // Заполняем диалог текущими данными продукта
        if (auto medicine = std::dynamic_pointer_cast<Medicine>(product)) {
            // Преобразуем строку с датой в QDate
            std::string expDateStr = product->getExpirationDate();
            QDate expiryDate = QDate::fromString(QString::fromStdString(expDateStr), "yyyy-MM-dd");

            // Если парсинг не удался, используем текущую дату + 2 года
            if (!expiryDate.isValid()) {
                expiryDate = QDate::currentDate().addYears(2);
            }

            if (auto tablet = std::dynamic_pointer_cast<Tablet>(product)) {
                dialog->setProductData(
                    QString::fromStdString(product->getId()),
                    QString::fromStdString(product->getName()),
                    product->getBasePrice(),
                    QString::fromStdString(medicine->getDosageForm()),
                    expiryDate,
                    QString::fromStdString(product->getManufacturerCountry()),
                    medicine->getIsPrescription(),
                    QString::fromStdString(medicine->getActiveSubstance()),
                    QString::fromStdString(medicine->getInstructions()),
                    // Специфичные поля для таблеток
                    tablet->getUnitsPerPackage(),
                    tablet->getDosageMg(),
                    QString::fromStdString(tablet->getCoating())
                    );
            }
            else if (auto syrup = std::dynamic_pointer_cast<Syrup>(product)) {
                dialog->setProductData(
                    QString::fromStdString(product->getId()),
                    QString::fromStdString(product->getName()),
                    product->getBasePrice(),
                    QString::fromStdString(medicine->getDosageForm()),
                    expiryDate,
                    QString::fromStdString(product->getManufacturerCountry()),
                    medicine->getIsPrescription(),
                    QString::fromStdString(medicine->getActiveSubstance()),
                    QString::fromStdString(medicine->getInstructions()),
                    // Специфичные поля для сиропа (таблеточные параметры - по умолчанию)
                    0, 0.0, "",
                    syrup->getVolumeMl(),
                    syrup->getHasSugar(),
                    QString::fromStdString(syrup->getFlavor())
                    );
            }
            else if (auto ointment = std::dynamic_pointer_cast<Ointment>(product)) {
                dialog->setProductData(
                    QString::fromStdString(product->getId()),
                    QString::fromStdString(product->getName()),
                    product->getBasePrice(),
                    QString::fromStdString(medicine->getDosageForm()),
                    expiryDate,
                    QString::fromStdString(product->getManufacturerCountry()),
                    medicine->getIsPrescription(),
                    QString::fromStdString(medicine->getActiveSubstance()),
                    QString::fromStdString(medicine->getInstructions()),
                    // Специфичные поля для мази (таблеточные и сиропные параметры - по умолчанию)
                    0, 0.0, "",
                    0.0, false, "",
                    ointment->getWeightG(),
                    QString::fromStdString(ointment->getBaseType())
                    );
            }
        }

        // Показываем диалог и ждем результат
        if (dialog->exec() == QDialog::Accepted) {
            // Создаем обновленный продукт на основе данных из диалога
            std::shared_ptr<MedicalProduct> updatedProduct;

            QString type = dialog->getType();
            QString id = dialog->getId();
            QString name = dialog->getName();
            double price = dialog->getPrice();
            QDate expiryDate = dialog->getExpiryDate();
            QString country = dialog->getCountry();
            bool prescription = dialog->isPrescription();
            QString substance = dialog->getSubstance();
            QString instructions = dialog->getInstructions();

            SafeDate safeDate(expiryDate.year(), expiryDate.month(), expiryDate.day());

            // Создаем продукт соответствующего типа ИСПОЛЬЗУЯ ДАННЫЕ ИЗ ДИАЛОГА
            if (type == "Таблетки") {
                updatedProduct = std::make_shared<Tablet>(
                    id.toStdString(), name.toStdString(), price,
                    safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getTabletUnits(),        // ИЗ ДИАЛОГА
                    dialog->getTabletDosage(),       // ИЗ ДИАЛОГА
                    dialog->getTabletCoating().toStdString() // ИЗ ДИАЛОГА
                    );
            } else if (type == "Сироп") {
                updatedProduct = std::make_shared<Syrup>(
                    id.toStdString(), name.toStdString(), price,
                    safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getSyrupVolume(),        // ИЗ ДИАЛОГА
                    dialog->getSyrupHasSugar(),      // ИЗ ДИАЛОГА
                    dialog->getSyrupFlavor().toStdString() // ИЗ ДИАЛОГА
                    );
            } else if (type == "Мазь") {
                updatedProduct = std::make_shared<Ointment>(
                    id.toStdString(), name.toStdString(), price,
                    safeDate, country.toStdString(),
                    prescription, substance.toStdString(), instructions.toStdString(),
                    dialog->getOintmentWeight(),     // ИЗ ДИАЛОГА
                    dialog->getOintmentBase().toStdString() // ИЗ ДИАЛОГА
                    );
            }

            // Обновляем продукт в менеджере
            if (updatedProduct) {
                pharmacyManager.updateProduct(updatedProduct);

                // Обновляем отображение
                QString productName = QString::fromStdString(updatedProduct->getName());
                showProductDetails(productId, productName);
                showProductList(); // Обновляем список продуктов

                QMessageBox::information(this, "Успех", "Изменения сохранены");
            } else {
                QMessageBox::warning(this, "Ошибка", "Не удалось обновить продукт");
            }
        }

        delete dialog; // Используйте delete вместо deleteLater для диалогов

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Ошибка при сохранении изменений: %1").arg(e.what()));
    }
}

void MainWindow::fillDialogWithProductData(AddProductDialog *dialog, const QString& productId, const QString& productName)
{
    // База данных продуктов (должна быть синхронизирована с showProductDetails)
    QMap<QString, QMap<QString, QString>> productsData;

    productsData["M001"] = {
        {"name", "Парацетамол"},
        {"price", "15.50"},
        {"country", "Россия"},
        {"expiry", "2025-12-31"},
        {"status", "В наличии"},
        {"substance", "Парацетамол"},
        {"form", "Таблетки"},
        {"usage", "По 1 таблетке 3 раза в день"}
    };

    productsData["M002"] = {
        {"name", "Ибупрофен"},
        {"price", "18.75"},
        {"country", "Россия"},
        {"expiry", "2025-10-15"},
        {"status", "В наличии"},
        {"substance", "Ибупрофен"},
        {"form", "Таблетки"},
        {"usage", "По 1 таблетке 2-3 раза в день"}
    };

    // Добавьте остальные продукты...

    if (productsData.contains(productId)) {
        QMap<QString, QString> productInfo = productsData[productId];

        // Преобразуем данные для диалога
        double price = productInfo["price"].toDouble();
        QDate expiryDate = QDate::fromString(productInfo["expiry"], "yyyy-MM-dd");
        bool prescription = false; // Можно добавить в данные

        // Заполняем диалог
        dialog->setProductData(
            productId,
            productInfo["name"],
            price,
            productInfo["form"], // type
            expiryDate,
            productInfo["country"],
            prescription,
            productInfo["substance"],
            productInfo["usage"]
            );
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
            pharmacyManager.removeProduct(productId.toStdString());
            delete productsList->takeItem(productsList->row(item));
            updateCompleter();

            contentText->setVisible(true);
            contentText->setPlainText("Выберите лекарство из списка для просмотра информации.");

        } catch (const std::exception& e) {
            QMessageBox::warning(this, "Ошибка", QString("Ошибка удаления: %1").arg(e.what()));
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
