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
    if (!productsList || !contentText) return;

    // Скрываем список, показываем детальную информацию
    productsList->setVisible(false);
    contentText->setVisible(true);

    contentText->clear();
    contentText->setHtml(
        QString("<h2 style='color: #2E7D32;'>%1</h2>"
                "<table width='100%' cellpadding='5'>"
                "<tr><td width='30%'><b>ID:</b></td><td>%2</td></tr>"
                "<tr><td><b>Название:</b></td><td>%1</td></tr>"
                "<tr><td><b>Цена:</b></td><td>15.50 руб.</td></tr>"
                "<tr><td><b>Страна:</b></td><td>Россия</td></tr>"
                "<tr><td><b>Срок годности:</b></td><td>2025-12-31</td></tr>"
                "<tr><td><b>Статус:</b></td><td><span style='color: #4CAF50;'>✓ В наличии</span></td></tr>"
                "<tr><td><b>Активное вещество:</b></td><td>%3</td></tr>"
                "<tr><td><b>Форма выпуска:</b></td><td>Таблетки</td></tr>"
                "<tr><td><b>Способ применения:</b></td><td>По 1 таблетке 3 раза в день</td></tr>"
                "</table>"
                "<br><button style='background-color: #4CAF50; color: white; padding: 10px; border: none; border-radius: 5px;' onclick='alert(\"Режим редактирования\")'>Изменить</button>")
            .arg(productName)
            .arg(productId)
            .arg(productName) // временно используем имя как активное вещество
        );
}

void MainWindow::showEditPanel()
{
    if (editPanel) {
        editPanel->setVisible(true);
        isEditMode = true;
    }
}

void MainWindow::hideEditPanel()
{
    if (editPanel) {
        editPanel->setVisible(false);
        isEditMode = false;
    }
}

void MainWindow::updateCompleter()
{
    QStringList sampleData = {
        "Парацетамол", "Ибупрофен", "Аспирин", "Амбробене", "Нурофен",
        "Анальгин", "Цитрамон", "Но-шпа", "Валидол", "Корвалол",
        "M001", "M002", "M003", "M004", "M005", "M006", "M007",
        "Россия", "Германия", "США", "Словения", "Великобритания", "Франция"
    };

    searchCompleter->setModel(new QStringListModel(sampleData, searchCompleter));
}

// ===== SLOTS =====

void MainWindow::onSearchTextChanged(const QString &text)
{
    currentFilter = text;
}

void MainWindow::onSearchButtonClicked()
{
    onSearchEnterPressed();
}

void MainWindow::onSearchEnterPressed()
{
    if (currentFilter.isEmpty()) {
        onShowAllProducts();
    } else {
        showProductList();
        // Здесь будет фильтрация списка по currentFilter
        QMessageBox::information(this, "Поиск",
                                 QString("Поиск по запросу: \"%1\"\n(функциональность будет реализована)").arg(currentFilter));
    }
}

void MainWindow::onShowAllProducts()
{
    currentFilter.clear();
    if (searchEdit) {
        searchEdit->clear();
    }
    showProductList();
    hideEditPanel();
}

void MainWindow::onProductSelected(QListWidgetItem *item)
{
    if (!item) return;

    QString productId = item->data(Qt::UserRole).toString();
    QString productName = item->text().left(item->text().indexOf(" ("));

    showProductDetails(productId, productName);
    hideEditPanel(); // Скрываем панель редактирования при выборе нового продукта
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
    showEditPanel();
    QMessageBox::information(this, "Редактирование",
                             "Панель редактирования активирована\n(функциональность будет реализована)");
}

void MainWindow::onDeleteProduct()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Удаление",
                                                              "Вы уверены, что хотите удалить этот продукт?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QMessageBox::information(this, "Удаление", "Продукт удален (заглушка)");
        hideEditPanel();
        onShowAllProducts(); // Возвращаемся к списку
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
