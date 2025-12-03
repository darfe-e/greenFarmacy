// simpleavailabilitydialog.h
#ifndef SIMPLEAVAILABILITYDIALOG_H
#define SIMPLEAVAILABILITYDIALOG_H

#include <QDialog>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>

class SimpleAvailabilityDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SimpleAvailabilityDialog(const std::string& productId,
                                      const std::string& productName,
                                      QWidget* parent = nullptr);

private:
    void setupUI();
    void loadAvailabilityData();

    std::string productId;
    std::string productName;

    QTableWidget* tableWidget;
    QLabel* titleLabel;
};

#endif // SIMPLEAVAILABILITYDIALOG_H
