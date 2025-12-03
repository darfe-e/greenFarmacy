#include "simpleavailabilitydialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include "filemanager.h"
#include "pharmacy.h"
#include <algorithm>

SimpleAvailabilityDialog::SimpleAvailabilityDialog(const std::string& productId,
                                                   const std::string& productName,
                                                   QWidget* parent)
    : QDialog(parent),
    productId(productId),
    productName(productName),
    tableWidget(nullptr),
    titleLabel(nullptr)
{
    setWindowTitle("Наличие в других аптеках");
    setMinimumSize(700, 500);

    setupUI();
    loadAvailabilityData();
}

void SimpleAvailabilityDialog::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 15, 15, 15);

    // Заголовок
    titleLabel = new QLabel(QString("<div style='text-align: center;'>"
                                    "<h2 style='color: #2E7D32; margin: 5px;'>Наличие: %1</h2>"
                                    "<p style='color: #666; font-size: 14px;'>ID лекарства: %2</p>"
                                    "</div>")
                                .arg(QString::fromStdString(productName))
                                .arg(QString::fromStdString(productId)));
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Таблица с наличием
    tableWidget = new QTableWidget();
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels(QStringList() << "Аптека" << "Адрес" << "Телефон" << "Количество");

    // Настройка заголовков
    QHeaderView* header = tableWidget->horizontalHeader();
    header->setSectionResizeMode(0, QHeaderView::Stretch); // Название
    header->setSectionResizeMode(1, QHeaderView::Stretch); // Адрес
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Телефон
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents); // Количество

    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // ВАЖНО: Отключаем выделение полностью!
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    // Также отключаем фокус у таблицы
    tableWidget->setFocusPolicy(Qt::NoFocus);

    tableWidget->setAlternatingRowColors(true);
    tableWidget->setShowGrid(false);

    // Настройка внешнего вида
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
            color: #333333;  /* Явно задаем цвет текста */
        }
        /* Убираем стили для выделения и наведения */
        QTableWidget::item:selected {
            background-color: transparent;  /* Прозрачный фон при выделении */
            color: #333333;  /* Оставляем тот же цвет текста */
        }
        QTableWidget::item:hover {
            background-color: transparent;  /* Убираем изменение фона при наведении */
        }
        /* Стиль для четных строк (чередование цветов) */
        QTableWidget::item:nth-child(even) {
            background-color: #f9f9f9;
        }
        /* Стиль для нечетных строк */
        QTableWidget::item:nth-child(odd) {
            background-color: white;
        }
    )");

    // Установим высоту строк
    tableWidget->verticalHeader()->setDefaultSectionSize(45);

    // Отключаем возможность выделения текста в ячейках
    tableWidget->setTextElideMode(Qt::ElideRight);

    // Устанавливаем, что ячейки не могут быть выбраны
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);

    mainLayout->addWidget(tableWidget, 1); // Растягиваем таблицу

    // Кнопка закрытия
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
        QPushButton:pressed {
            background-color: #3d8b40;
        }
    )");

    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);
    buttonLayout->addStretch();

    mainLayout->addLayout(buttonLayout);

    // Устанавливаем, что таблица не принимает фокус при клике
    tableWidget->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    tableWidget->setFocusPolicy(Qt::NoFocus);

    // Устанавливаем флаг, чтобы ячейки не могли быть выбраны
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
}
void SimpleAvailabilityDialog::loadAvailabilityData()
{
    tableWidget->setRowCount(0);
    tableWidget->clearContents();

    qDebug() << "\n=== SimpleAvailabilityDialog::loadAvailabilityData() ===";
    qDebug() << "Лекарство:" << QString::fromStdString(productName);
    qDebug() << "ID:" << QString::fromStdString(productId);

    try {
        // Получаем наличие в других аптеках
        auto availability = FileManager::getInstance()
                                .getAvailabilityInOtherPharmacies(productId);

        qDebug() << "Получено результатов от FileManager:" << availability.size();

        if (availability.empty()) {
            qDebug() << "Нет данных, показываем сообщение";
            tableWidget->setRowCount(1);
            QTableWidgetItem* item = new QTableWidgetItem(
                "😕 Лекарство отсутствует в других аптеках");
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QColor("#F44336"));
            item->setFont(QFont("Arial", 12, QFont::Bold));
            tableWidget->setSpan(0, 0, 1, 4);
            tableWidget->setItem(0, 0, item);
            return;
        }

        // Сортируем по количеству (от большего к меньшему)
        std::sort(availability.begin(), availability.end(),
                  [](const auto& a, const auto& b) {
                      return a.second > b.second;
                  });

        qDebug() << "Начинаем заполнение таблицы, записей:" << availability.size();

        // Заполняем таблицу
        int row = 0;
        for (const auto& pair : availability) {
            // Получаем данные
            std::string pharmacyInfo = pair.first;  // Формат: "Название\nАдрес\nТелефон"
            int quantity = pair.second;

            qDebug() << "  Обрабатываем запись" << row
                     << ":" << QString::fromStdString(pharmacyInfo)
                     << "-" << quantity << "шт.";

            tableWidget->insertRow(row);

            // Парсим строку с информацией об аптеке
            // Формат: "Название\nАдрес\nТелефон" или "Аптека №XXX" (если не нашли аптеку)
            QString info = QString::fromStdString(pharmacyInfo);
            QStringList parts = info.split("\n");

            QString name, address, phone;

            if (parts.size() >= 3) {
                // Полный формат: название, адрес, телефон
                name = parts[0];
                address = parts[1];
                phone = parts[2];
            }
            else if (parts.size() == 2) {
                // Только название и адрес
                name = parts[0];
                address = parts[1];
                phone = "Телефон не указан";
            }
            else if (parts.size() == 1) {
                // Только название (Аптека №XXX)
                name = parts[0];
                address = "Адрес не указан";
                phone = "Телефон не указан";
            }
            else {
                name = "Неизвестная аптека";
                address = "Адрес не указан";
                phone = "Телефон не указан";
            }

            // Название аптеки
            QTableWidgetItem* nameItem = new QTableWidgetItem(name);
            nameItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            nameItem->setFlags(nameItem->flags() & ~Qt::ItemIsEditable);
            tableWidget->setItem(row, 0, nameItem);

            // Адрес
            QTableWidgetItem* addressItem = new QTableWidgetItem(address);
            addressItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            addressItem->setFlags(addressItem->flags() & ~Qt::ItemIsEditable);
            tableWidget->setItem(row, 1, addressItem);

            // Телефон (СЕЙЧАС ОТОБРАЖАЕТСЯ!)
            QTableWidgetItem* phoneItem = new QTableWidgetItem(phone);
            phoneItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            phoneItem->setFlags(phoneItem->flags() & ~Qt::ItemIsEditable);
            tableWidget->setItem(row, 2, phoneItem);

            // Количество
            QTableWidgetItem* quantityItem = new QTableWidgetItem(QString::number(quantity));
            quantityItem->setTextAlignment(Qt::AlignCenter);
            quantityItem->setFlags(quantityItem->flags() & ~Qt::ItemIsEditable);

            // Подсветка в зависимости от количества
            if (quantity < 5) {
                quantityItem->setBackground(QColor(255, 235, 238));
                quantityItem->setForeground(QColor(198, 40, 40));
                quantityItem->setToolTip("Очень мало в наличии (< 5)");
            } else if (quantity < 20) {
                quantityItem->setBackground(QColor(255, 249, 196));
                quantityItem->setForeground(QColor(255, 152, 0));
                quantityItem->setToolTip("Мало в наличии (5-20)");
            } else if (quantity < 50) {
                quantityItem->setBackground(QColor(232, 245, 233));
                quantityItem->setForeground(QColor(46, 125, 50));
                quantityItem->setToolTip("Среднее количество (20-50)");
            } else {
                quantityItem->setBackground(QColor(200, 230, 201));
                quantityItem->setForeground(QColor(27, 94, 32));
                quantityItem->setToolTip("Много в наличии (> 50)");
            }

            tableWidget->setItem(row, 3, quantityItem);

            // Добавляем всплывающие подсказки
            nameItem->setToolTip("Название аптеки");
            addressItem->setToolTip("Адрес аптеки");
            phoneItem->setToolTip("Контактный телефон аптеки");

            // Если телефон есть, делаем его более заметным
            if (phone != "Телефон не указан") {
                phoneItem->setForeground(QColor("#1E88E5"));
                phoneItem->setToolTip("Позвоните по этому номеру!");
            }

            row++;
        }

        qDebug() << "Таблица заполнена успешно, строк:" << tableWidget->rowCount();

        // Автоматически подгоняем ширину столбцов
        tableWidget->resizeColumnsToContents();

        // Установим минимальную ширину для столбцов
        tableWidget->setColumnWidth(0, 150); // Название
        tableWidget->setColumnWidth(1, 200); // Адрес
        tableWidget->setColumnWidth(2, 140); // Телефон (увеличил для полного номера)
        tableWidget->setColumnWidth(3, 100); // Количество

    } catch (const std::exception& e) {
        qDebug() << "❌ ОШИБКА при загрузке данных:" << e.what();

        tableWidget->setRowCount(1);
        QTableWidgetItem* errorItem = new QTableWidgetItem(
            QString("⚠️ Ошибка загрузки данных: %1").arg(e.what()));
        errorItem->setTextAlignment(Qt::AlignCenter);
        errorItem->setForeground(QColor("#F44336"));
        errorItem->setFont(QFont("Arial", 10, QFont::Normal));
        tableWidget->setSpan(0, 0, 1, 4);
        tableWidget->setItem(0, 0, errorItem);
    }

    qDebug() << "=== Конец loadAvailabilityData() ===";
}
