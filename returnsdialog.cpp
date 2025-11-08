#include "returnsdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

ReturnsDialog::ReturnsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void ReturnsDialog::setupUI()
{
    setWindowTitle("Управление возвратами");
    setMinimumSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Управление возвратами лекарств");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *infoLabel = new QLabel(
        "Это окно для управления возвратами лекарственных препаратов.\n\n"
        "Здесь будут доступны:\n"
        "• Регистрация возвратов от аптек\n"
        "• Учет причин возвратов\n"
        "• Статусы обработки возвратов\n"
        "• Отчеты по возвратам\n\n"
        "Функциональность находится в разработке."
        );
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    QPushButton *saveButton = new QPushButton("Закрыть");
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &ReturnsDialog::onSave);
}

void ReturnsDialog::onSave()
{
    accept();
}
