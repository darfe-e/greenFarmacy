#include "analoguesdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

AnaloguesDialog::AnaloguesDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void AnaloguesDialog::setupUI()
{
    setWindowTitle("Поиск аналогов");
    setMinimumSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Поиск аналогов лекарств");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *infoLabel = new QLabel(
        "Это окно для поиска аналогов лекарственных препаратов.\n\n"
        "Здесь будут доступны:\n"
        "• Поиск по активному веществу\n"
        "• Сравнение цен аналогов\n"
        "• Информация о взаимозаменяемости\n"
        "• Наличие аналогов в аптеках\n"
        "• Подбор замены по показаниям\n\n"
        "Функциональность находится в разработке."
        );
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    QPushButton *saveButton = new QPushButton("Закрыть");
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &AnaloguesDialog::onSave);
}

void AnaloguesDialog::onSave()
{
    accept();
}
