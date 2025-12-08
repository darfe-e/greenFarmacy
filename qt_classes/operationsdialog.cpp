#include "operationsdialog.h"
#include "my_inheritence/pharmacymanager.h"
#include "my_inheritence/supply.h"
#include "my_inheritence/return.h"
#include "my_inheritence/writeoff.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>

OperationsDialog::OperationsDialog(PharmacyManager& manager, OperationType type, QWidget *parent)
    : QDialog(parent)
    , pharmacyManager(manager)
    , currentType(type)
    , tableWidget(nullptr)
    , titleLabel(nullptr)
    , typeComboBox(nullptr)
{
    setWindowTitle(getWindowTitle());
    setMinimumSize(1000, 600);

    setupUI();
    loadOperationsData();
}

void OperationsDialog::setupUI()                               // Настройка интерфейса диалога
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    QHBoxLayout* headerLayout = new QHBoxLayout();

    titleLabel = new QLabel(QString("<h2 style='color: #2E7D32; margin: 5px;'>%1</h2>")
                                .arg(getWindowTitle()));
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    typeComboBox = new QComboBox();
    typeComboBox->addItem("📦 Поставки", SUPPLY);
    typeComboBox->addItem("🔄 Возвраты", RETURN);
    typeComboBox->addItem("🗑️ Списания", WRITEOFF);
    typeComboBox->setCurrentIndex(static_cast<int>(currentType));
    typeComboBox->setStyleSheet(R"(
        QComboBox {
            padding: 8px;
            border: 2px solid #4CAF50;
            border-radius: 6px;
            font-size: 14px;
            min-width: 150px;
        }
    )");

    connect(typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OperationsDialog::onOperationTypeChanged);

    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(new QLabel("Тип операций:"));
    headerLayout->addWidget(typeComboBox);

    mainLayout->addLayout(headerLayout);

    tableWidget = new QTableWidget();
    updateTableColumns();

    QHeaderView* header = tableWidget->horizontalHeader();
    header->setStretchLastSection(true);
    header->setSectionResizeMode(QHeaderView::Stretch);
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->setAlternatingRowColors(true);

    tableWidget->setStyleSheet(R"(
        QTableWidget {
            border: 2px solid #4CAF50;
            border-radius: 8px;
            background-color: white;
            font-size: 13px;
        }
        QHeaderView::section {
            background-color: #4CAF50;
            color: white;
            padding: 10px;
            border: none;
            font-weight: bold;
            font-size: 14px;
        }
        QTableWidget::item {
            padding: 8px;
            border-bottom: 1px solid #e0e0e0;
            color: #333333;
        }
        QTableWidget::item:nth-child(even) {
            background-color: #f9f9f9;
        }
        QTableWidget::item:nth-child(odd) {
            background-color: white;
        }
    )");

    tableWidget->verticalHeader()->setDefaultSectionSize(45);
    mainLayout->addWidget(tableWidget, 1);

    QLabel* infoLabel = nullptr;
    switch (currentType)
    {
    case SUPPLY:
        infoLabel = new QLabel("💡 <b>Поставки</b> добавляются только через файл. Для добавления новой поставки отредактируйте файл operations.txt");
        break;
    case RETURN:
        infoLabel = new QLabel("💡 <b>Возвраты</b> создаются автоматически при удалении лекарств из каталога");
        break;
    case WRITEOFF:
        infoLabel = new QLabel("💡 <b>Списания</b> создаются автоматически при загрузке просроченных лекарств");
        break;
    }

    if (infoLabel)
    {
        infoLabel->setWordWrap(true);
        infoLabel->setStyleSheet("color: #666; padding: 10px; background-color: #f0f9ff; border-radius: 6px;");
        mainLayout->addWidget(infoLabel);
    }

    QPushButton* closeButton = new QPushButton("✕ Закрыть");
    closeButton->setMinimumSize(120, 40);
    closeButton->setStyleSheet(R"(
        QPushButton {
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 6px;
            font-weight: bold;
            font-size: 14px;
            padding: 8px 20px;
        }
        QPushButton:hover {
            background-color: #45a049;
        }
    )");

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(closeButton, &QPushButton::clicked, this, &OperationsDialog::onClose);
}

void OperationsDialog::updateTableColumns()                    // Обновление столбцов таблицы
{
    tableWidget->clear();
    tableWidget->setRowCount(0);

    switch (currentType)
    {
    case SUPPLY:
        tableWidget->setColumnCount(6);
        tableWidget->setHorizontalHeaderLabels(
            QStringList() << "ID операции" << "Дата" << "ID лекарства" << "Количество" << "Источник" << "Назначение");
        break;
    case RETURN:
        tableWidget->setColumnCount(6);
        tableWidget->setHorizontalHeaderLabels(
            QStringList() << "ID операции" << "Дата" << "ID лекарства" << "Количество" << "Причина" << "Статус");
        break;
    case WRITEOFF:
        tableWidget->setColumnCount(6);
        tableWidget->setHorizontalHeaderLabels(
            QStringList() << "ID операции" << "Дата" << "ID лекарства" << "Количество" << "Причина" << "Статус");
        break;
    }

    for (int i = 0; i < tableWidget->columnCount(); ++i)
    {
        if (i == 0)
            tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        else
            tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
}

void OperationsDialog::loadOperationsData()                    // Загрузка данных операций
{
    tableWidget->setRowCount(0);

    try
    {
        std::vector<std::shared_ptr<InventoryOperation>> operations;

        switch (currentType)
        {
        case SUPPLY:
        {
            auto supplies = pharmacyManager.getSupplyOperations();
            operations.assign(supplies.begin(), supplies.end());
            break;
        }
        case RETURN:
        {
            auto returns = pharmacyManager.getReturnOperations();
            operations.assign(returns.begin(), returns.end());
            break;
        }
        case WRITEOFF:
        {
            auto writeOffs = pharmacyManager.getWriteOffOperations();
            operations.assign(writeOffs.begin(), writeOffs.end());
            break;
        }
        }

        for (size_t i = 0; i < operations.size(); ++i)
        {
            const auto& op = operations[i];
            tableWidget->insertRow(i);

            tableWidget->setItem(i, 0,
                                 new QTableWidgetItem(QString::fromStdString(op->getId())));

            tableWidget->setItem(i, 1,
                                 new QTableWidgetItem(QString::fromStdString(op->getOperationDate().toString())));

            tableWidget->setItem(i, 2,
                                 new QTableWidgetItem(QString::fromStdString(op->getProductId())));

            QTableWidgetItem* quantityItem = new QTableWidgetItem(QString::number(op->getQuantity()));
            quantityItem->setTextAlignment(Qt::AlignCenter);
            tableWidget->setItem(i, 3, quantityItem);

            if (currentType == SUPPLY)
            {
                if (auto supply = std::dynamic_pointer_cast<Supply>(op))
                {
                    tableWidget->setItem(i, 4,
                                         new QTableWidgetItem(QString::fromStdString(supply->getSource())));
                    tableWidget->setItem(i, 5,
                                         new QTableWidgetItem(QString::fromStdString(supply->getDestination())));
                }
            }
            else if (currentType == RETURN)
            {
                if (auto returnOp = std::dynamic_pointer_cast<Return>(op))
                {
                    tableWidget->setItem(i, 4,
                                         new QTableWidgetItem(QString::fromStdString(returnOp->getReason())));
                    tableWidget->setItem(i, 5,
                                         new QTableWidgetItem(QString::fromStdString(op->getStatus())));
                }
            }
            else if (currentType == WRITEOFF)
            {
                if (auto writeOff = std::dynamic_pointer_cast<WriteOff>(op))
                {
                    tableWidget->setItem(i, 4,
                                         new QTableWidgetItem(QString::fromStdString(writeOff->getWriteOffReason())));
                    tableWidget->setItem(i, 5,
                                         new QTableWidgetItem(QString::fromStdString(op->getStatus())));
                }
            }

            if (currentType == RETURN || currentType == WRITEOFF)
            {
                QTableWidgetItem* statusItem = tableWidget->item(i, 5);
                if (statusItem)
                {
                    QString status = statusItem->text();
                    if (status == "completed")
                    {
                        statusItem->setBackground(QColor(232, 245, 233));
                        statusItem->setForeground(QColor(46, 125, 50));
                    }
                    else if (status == "pending")
                    {
                        statusItem->setBackground(QColor(255, 249, 196));
                        statusItem->setForeground(QColor(255, 152, 0));
                    }
                    else
                    {
                        statusItem->setBackground(QColor(255, 235, 238));
                        statusItem->setForeground(QColor(198, 40, 40));
                    }
                }
            }

            if (op->getQuantity() < 10)
            {
                quantityItem->setBackground(QColor(255, 249, 196));
                quantityItem->setToolTip("Малое количество");
            }
        }

        setWindowTitle(QString("%1 (%2 записей)")
                           .arg(getWindowTitle())
                           .arg(operations.size()));

        titleLabel->setText(QString("<h2 style='color: #2E7D32; margin: 5px;'>%1 (%2 записей)</h2>")
                                .arg(getOperationTypeString())
                                .arg(operations.size()));
    }
    catch (const std::exception& e)
    {
        QMessageBox::warning(this, "Ошибка",
                             QString("Ошибка загрузки данных: %1").arg(e.what()));
    }
}

QString OperationsDialog::getOperationTypeString() const       // Получение строки типа операции
{
    switch (currentType)
    {
    case SUPPLY: return "Поставки";
    case RETURN: return "Возвраты";
    case WRITEOFF: return "Списания";
    default: return "Операции";
    }
}

QString OperationsDialog::getWindowTitle() const               // Получение заголовка окна
{
    return QString("Управление %1").arg(getOperationTypeString());
}

void OperationsDialog::onOperationTypeChanged(int index)       // Обработка изменения типа операции
{
    currentType = static_cast<OperationType>(typeComboBox->itemData(index).toInt());
    updateTableColumns();
    loadOperationsData();
}

void OperationsDialog::onClose()                               // Обработка закрытия диалога
{
    accept();
}
