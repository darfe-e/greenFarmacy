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

void MainWindow::showProductList()
{
    if (!productsList || !contentText) return;

    // Показываем список, скрываем текстовое поле
    productsList->setVisible(true);
    contentText->setVisible(false);

    productsList->clear();

    // Заглушка с тестовыми данными
    QStringList sampleProducts = {
        "Парацетамол (M001) - 15.50 руб.",
        "Ибупрофен (M002) - 18.75 руб.",
        "Аспирин (M003) - 12.30 руб.",
        "Амбробене (M004) - 45.30 руб.",
        "Нурофен (M005) - 22.40 руб.",
        "Анальгин (M006) - 8.90 руб.",
        "Цитрамон (M007) - 11.20 руб."
    };

    for (const QString& product : sampleProducts) {
        QListWidgetItem *item = new QListWidgetItem(product, productsList);
        item->setData(Qt::UserRole, product.mid(product.indexOf("(") + 1, 4));
    }
}

void MainWindow::showProductDetails(const QString& productId, const QString& productName)
{
    if (!contentText) return;

    // Скрываем список и показываем текстовое поле с деталями
    productsList->setVisible(false);
    contentText->setVisible(true);

    // Формируем детальную информацию о продукте
    QString details;

    if (productId == "M001") {
        details = QString(
            "# Информация\n\n"
            "## Парацетамол\n"
            "**ID:** M001\n"
            "**Название:** Парацетамол\n"
            "**Цена:** 15.50 руб.\n"
            "**Страна:** Россия\n"
            "**Срок годности:** 2025-12-31\n"
            "**Статус:** ✓ В наличии\n"
            "**Активное вещество:** Парацетамол\n"
            "**Форма выпуска:** Таблетки\n"
            "**Способ применения:** По 1 таблетке 3 раза в день\n\n"
            "[Изменение]"
            );
    } else if (productId == "M002") {
        details = QString(
            "# Информация\n\n"
            "## Ибупрофен\n"
            "**ID:** M002\n"
            "**Название:** Ибупрофен\n"
            "**Цена:** 18.75 руб.\n"
            "**Страна:** Россия\n"
            "**Срок годности:** 2025-10-15\n"
            "**Статус:** ✓ В наличии\n"
            "**Активное вещество:** Ибупрофен\n"
            "**Форма выпуска:** Таблетки\n"
            "**Способ применения:** По 1 таблетке 2-3 раза в день\n\n"
            "[Изменение]"
            );
    }
    // Добавьте остальные продукты по аналогии...

    contentText->setHtml("<pre>" + details + "</pre>");
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
    wordList << "Парацетамол" << "Ибупрофен" << "Аспирин" << "Амбробене"
             << "Нурофен" << "Анальгин" << "Цитрамон"
             << "M001" << "M002" << "M003" << "M004" << "M005" << "M006" << "M007";

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
    currentFilter.clear();
    if (searchEdit) {
        searchEdit->clear();
    }
    showProductList();
    hideEditPanel(); // Скрываем панель при показе всего списка

    // Сбрасываем текст контента
    if (contentText) {
        contentText->setVisible(false);
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
    AddProductDialog dialog(this);
    dialog.exec();
}

void MainWindow::onEditProduct()
{
    QListWidgetItem *item = productsList->currentItem();
    if (!item) return;

    QString productId = item->data(Qt::UserRole).toString();

    AddProductDialog *dialog = new AddProductDialog(this);
    dialog->setEditMode(true);
    fillDialogWithProductData(dialog, productId, "");

    if (dialog->exec() == QDialog::Accepted) {
        // Получаем обновленные данные из диалога
        QString newId = dialog->getId();
        QString newName = dialog->getName();
        double newPrice = dialog->getPrice();
        QString newType = dialog->getType();

        // Обновляем элемент в списке
        QString newText = QString("%1 (%2) - %3 руб.").arg(newName).arg(newId).arg(newPrice, 0, 'f', 2);
        item->setText(newText);
        item->setData(Qt::UserRole, newId);

        // Показываем сообщение об успехе ПОСЛЕ закрытия диалога
        QMessageBox::information(this, "Успех", "Данные лекарства обновлены!");

        // Обновляем детальную информацию
        onProductSelected(item);
    }

    delete dialog;
}

void MainWindow::saveProductChanges(const QString& productId)
{
    // Здесь должна быть логика сохранения изменений в вашей модели данных
    // Пока просто обновим отображение

    QListWidgetItem *item = productsList->currentItem();
    if (item) {
        // Обновляем текст в списке
        // В реальном приложении здесь нужно получить новые данные из диалога
        // и обновить элемент списка
        QMessageBox::information(this, "Сохранение",
                                 "Изменения для " + productId + " сохранены (заглушка)");

        // Обновляем детальную информацию
        onProductSelected(item);
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
    if (item) {
        QString productName = item->text().left(item->text().indexOf(" ("));
        int ret = QMessageBox::question(this, "Удаление",
                                        "Вы уверены, что хотите удалить:\n" + productName + "?",
                                        QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::Yes) {
            delete productsList->takeItem(productsList->row(item));
            hideEditPanel();
            contentText->setVisible(true);
            contentText->setPlainText("Выберите лекарство из списка для просмотра информации.");
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
    if (!productsList || !contentText) return;

    // Показываем список, скрываем детальную информацию
    productsList->setVisible(true);
    contentText->setVisible(false);
    hideEditPanel();

    productsList->clear();
    productsList->clearSelection();

    // Тестовые данные для поиска
    QMap<QString, QString> sampleProducts = {
        {"M001", "Парацетамол (M001) - 15.50 руб."},
        {"M002", "Ибупрофен (M002) - 18.75 руб."},
        {"M003", "Аспирин (M003) - 12.30 руб."},
        {"M004", "Амбробене (M004) - 45.30 руб."},
        {"M005", "Нурофен (M005) - 22.40 руб."},
        {"M006", "Анальгин (M006) - 8.90 руб."},
        {"M007", "Цитрамон (M007) - 11.20 руб."}
    };

    // Поиск по названию и ID
    QString searchLower = searchText.toLower();
    bool found = false;

    for (auto it = sampleProducts.begin(); it != sampleProducts.end(); ++it) {
        QString productText = it.value();
        if (productText.toLower().contains(searchLower)) {
            QListWidgetItem *item = new QListWidgetItem(productText, productsList);
            item->setData(Qt::UserRole, it.key());
            found = true;
        }
    }

    if (!found) {
        // Если ничего не найдено
        productsList->setVisible(false);
        contentText->setVisible(true);
        contentText->setHtml(
            "<div style='text-align: center; padding: 50px; color: #666;'>"
            "<h3 style='color: #F44336;'>Ничего не найдено</h3>"
            "<p>По запросу: <b>\"" + searchText + "\"</b></p>"
                           "<p>Попробуйте изменить запрос или посмотрите все лекарства</p>"
                           "</div>"
            );
    } else {
        // Показываем заголовок с результатами поиска
        productsList->setVisible(true);
        contentText->setVisible(false);
    }
}

