#include "returnsdialog.h"
#include "pharmacymanager.h"
#include "return.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDate>

ReturnsDialog::ReturnsDialog(PharmacyManager& manager, QWidget *parent)
    : QDialog(parent)
    , pharmacyManager(manager)
    , tableWidget(nullptr)
    , titleLabel(nullptr)
{
    setWindowTitle("Управление возвратами");
    setMinimumSize(900, 600);

    setupUI();
    loadReturnsData();
}

void ReturnsDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Заголовок
    titleLabel = new QLabel(QString("<div style='text-align: center;'>"
                                    "<h2 style='color: #2E7D32; margin: 5px;'>Операции возвратов</h2>"
                                    "<p style='color: #666; font-size: 14px;'>Всего операций: 0</p>"
                                    "</div>"));
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Таблица
    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(6);
    tableWidget->setHorizontalHeaderLabels(
        QStringList() << "ID операции" << "Дата" << "Лекарство" << "Количество" << "Причина" << "Статус");

    // Настройка заголовков
    QHeaderView* header = tableWidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::ResizeToContents); // ID
    header->setSectionResizeMode(1, QHeaderView::ResizeToContents); // Дата
    header->setSectionResizeMode(2, QHeaderView::Stretch); // Лекарство
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Количество
    header->setSectionResizeMode(4, QHeaderView::Stretch); // Причина
    header->setSectionResizeMode(5, QHeaderView::ResizeToContents); // Статус

    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->setAlternatingRowColors(true);

    // Стилизация
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

    // Кнопки
    QHBoxLayout* buttonLayout = new QHBoxLayout();

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

    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    connect(closeButton, &QPushButton::clicked, this, &ReturnsDialog::onClose);
}

void ReturnsDialog::loadReturnsData()
{
    tableWidget->setRowCount(0);

    try {
        auto operations = pharmacyManager.getAllOperations();
        int returnCount = 0;

        for (const auto& op : operations) {
            if (op->getOperationType() == "Return") {
                if (auto returnOp = std::dynamic_pointer_cast<Return>(op)) {
                    tableWidget->insertRow(returnCount);

                    // ID операции
                    tableWidget->setItem(returnCount, 0,
                                         new QTableWidgetItem(QString::fromStdString(returnOp->getId())));

                    // Дата
                    tableWidget->setItem(returnCount, 1,
                                         new QTableWidgetItem(QString::fromStdString(returnOp->getOperationDate().toString())));

                    // Лекарство
                    tableWidget->setItem(returnCount, 2,
                                         new QTableWidgetItem(QString::fromStdString(returnOp->getProductId())));

                    // Количество
                    tableWidget->setItem(returnCount, 3,
                                         new QTableWidgetItem(QString::number(returnOp->getQuantity())));

                    // Причина
                    tableWidget->setItem(returnCount, 4,
                                         new QTableWidgetItem(QString::fromStdString(returnOp->getReason())));

                    // Статус
                    QTableWidgetItem* statusItem = new QTableWidgetItem(
                        QString::fromStdString(returnOp->getStatus()));

                    if (returnOp->getStatus() == "completed") {
                        statusItem->setBackground(QColor(232, 245, 233));
                        statusItem->setForeground(QColor(46, 125, 50));
                    } else if (returnOp->getStatus() == "pending") {
                        statusItem->setBackground(QColor(255, 249, 196));
                        statusItem->setForeground(QColor(255, 152, 0));
                    } else {
                        statusItem->setBackground(QColor(255, 235, 238));
                        statusItem->setForeground(QColor(198, 40, 40));
                    }

                    tableWidget->setItem(returnCount, 5, statusItem);

                    returnCount++;
                }
            }
        }

        titleLabel->setText(QString("<div style='text-align: center;'>"
                                    "<h2 style='color: #2E7D32; margin: 5px;'>Операции возвратов</h2>"
                                    "<p style='color: #666; font-size: 14px;'>Всего операций: %1</p>"
                                    "</div>").arg(returnCount));

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Ошибка",
                             QString("Ошибка загрузки данных: %1").arg(e.what()));
    }
}

void ReturnsDialog::onClose()
{
    accept();
}
