#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QCompleter>
#include "addproductdialog.h"
#include "my_inheritence/medicalproduct.h"
#include "my_inheritence/pharmacymanager.h"
#include "my_inheritence/tablet.h"
#include "my_inheritence/ointment.h"
#include "my_inheritence/syrup.h"
#include "my_inheritence/filemanager.h"
#include <QCloseEvent>
#include "simpleavailabilitydialog.h"

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
    void onShowOperations();
    void onShowAnalogues();
    void onShowPharmacyAvailability();
    void onAddProduct();
    void onEditProduct();
    void onDeleteProduct();
    void onSaveChanges();
    void onCancelEdit();
    void onUndo();

    void onItemSelected();
    void onAddAnalogue();
    void showProductDetailsInDialog(const QString& productId, QTextEdit* textEdit);
    std::string generateOperationId();

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
    void saveProductChanges(const QString& productId);
    void setupConnections();
    void loadAllData();
    QString formatProductDetails(std::shared_ptr<MedicalProduct> product);
    void closeEvent(QCloseEvent *event);
    void updateActionButtons();


    bool dataModified; // Флаг изменений данных
    bool isClosing; // Флаг закрытия программы

    // UI Components
    QLineEdit *searchEdit;
    QCompleter *searchCompleter;
    QPushButton *searchButton;
    QPushButton *showAllButton;

    // Navigation
    QPushButton *operationsBtn;
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
    QPushButton *addAnalogueBtn;
    QPushButton *viewAnaloguesBtn;

    QStack<QString> undoStack;
    QString currentFilter;
    bool isEditMode;


    QLineEdit *searchLineEdit;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *saveButton;
    QPushButton *cancelButton;

    // Менеджер данных
    PharmacyManager pharmacyManager;
    std::shared_ptr<MedicalProduct> currentEditingProduct;
    std::shared_ptr<MedicalProduct> currentProduct;

};

#endif // MAINWINDOW_H
