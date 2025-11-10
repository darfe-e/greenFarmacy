#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QCompleter>
#include "addproductdialog.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QListWidgetItem;
class QTextEdit;
class QLineEdit;
class QPushButton;
class QLabel;
class QGroupBox;
class QWidget;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSearchTextChanged(const QString &text);
    void onSearchButtonClicked();
    void onSearchEnterPressed();
    void onShowAllProducts();
    void onProductSelected(QListWidgetItem *item);
    void onShowSupplies();
    void onShowReturns();
    void onShowWriteOffs();
    void onShowAnalogues();
    void onShowPharmacyAvailability();
    void onAddProduct();
    void onEditProduct();
    void onDeleteProduct();
    void onSaveChanges();
    void onCancelEdit();
    void onUndo();
    void onItemSelected();

private:
    void setupUI();
    void setupColors();
    void showProductList();
    void showProductDetails(const QString& productId, const QString& productName);
    void showEditPanel();
    void hideEditPanel();
    void updateCompleter();
    void pushToUndoStack();
    void performSearch(const QString &searchText);
    void fillDialogWithProductData(AddProductDialog *dialog, const QString& productId, const QString& productName);
    void saveProductChanges(const QString& productId);

    // UI Components
    QLineEdit *searchEdit;
    QCompleter *searchCompleter;
    QPushButton *searchButton;
    QPushButton *showAllButton;

    // Navigation
    QPushButton *suppliesBtn;
    QPushButton *returnsBtn;
    QPushButton *writeOffsBtn;
    QPushButton *analoguesBtn;
    QPushButton *availabilityBtn;
    QPushButton *addProductBtn;

    // Actions
    QPushButton *saveBtn;
    QPushButton *undoBtn;

    // Main content
    QListWidget *productsList;
    QTextEdit *contentText;

    // Edit panel (right side)
    QWidget *editPanel;
    QPushButton *editBtn;
    QPushButton *deleteBtn;
    QPushButton *cancelEditBtn;

    QStack<QString> undoStack;
    QString currentFilter;
    bool isEditMode;
};

#endif // MAINWINDOW_H
