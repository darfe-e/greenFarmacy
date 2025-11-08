#include "suppliesdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

SuppliesDialog::SuppliesDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void SuppliesDialog::setupUI()
{
    setWindowTitle("Управление поставками");
    setMinimumSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Управление поставками лекарств");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *infoLabel = new QLabel(
        "Это окно для управления поставками лекарственных препаратов.\n\n"
        "Здесь будут доступны:\n"
        "• Просмотр текущих поставок\n"
        "• Добавление новых поставок\n"
        "• Отслеживание статуса поставок\n"
        "• Управление поставщиками\n"
        "• Отчеты по поставкам\n\n"
        "Функциональность находится в разработке."
        );
    infoLabel->setWordWrap(true);
    infoLabel->setAlignment(Qt::AlignLeft);
    mainLayout->addWidget(infoLabel);

    QPushButton *saveButton = new QPushButton("Закрыть");
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &SuppliesDialog::onSave);
}

void SuppliesDialog::onSave()
{
    accept();
}
