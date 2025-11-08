#include "writeoffsdialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

WriteOffsDialog::WriteOffsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
}

void WriteOffsDialog::setupUI()
{
    setWindowTitle("Управление списаниями");
    setMinimumSize(500, 400);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QLabel *titleLabel = new QLabel("Управление списаниями лекарств");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2E7D32; margin: 10px;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QLabel *infoLabel = new QLabel(
        "Это окно для управления списаниями лекарственных препаратов.\n\n"
        "Здесь будут доступны:\n"
        "• Списание просроченных лекарств\n"
        "• Списание бракованных препаратов\n"
        "• Учет причин списания\n"
        "• Отчеты по списаниям\n"
        "• Контроль сроков годности\n\n"
        "Функциональность находится в разработке."
        );
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    QPushButton *saveButton = new QPushButton("Закрыть");
    mainLayout->addWidget(saveButton);

    connect(saveButton, &QPushButton::clicked, this, &WriteOffsDialog::onSave);
}

void WriteOffsDialog::onSave()
{
    accept();
}
