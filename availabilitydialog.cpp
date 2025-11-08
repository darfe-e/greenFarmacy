#include "availabilitydialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

AvailabilityDialog::AvailabilityDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void AvailabilityDialog::setupUI()
{
    setWindowTitle("Наличие в аптеках");
    setMinimumSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Проверка наличия в аптеках");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *infoLabel = new QLabel(
        "Это окно для проверки наличия лекарств в аптеках сети.\n\n"
        "Здесь будут доступны:\n"
        "• Поиск лекарства по всем аптекам\n"
        "• Отображение остатков в реальном времени\n"
        "• Поиск ближайшей аптеки с нужным лекарством\n"
        "• Резервирование лекарств\n"
        "• Уведомления о поступлении\n\n"
        "Функциональность находится в разработке."
        );
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    QPushButton *saveButton = new QPushButton("Закрыть");
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &AvailabilityDialog::onSave);
}

void AvailabilityDialog::onSave()
{
    accept();
}
