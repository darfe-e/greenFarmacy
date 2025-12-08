QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Exception/safeinput.cpp \
    Files/file_txt.cpp \
    main.cpp \
    my_inheritence/filemanager.cpp \
    my_inheritence/inventoryoperation.cpp \
    my_inheritence/medicalproduct.cpp \
    my_inheritence/medicine.cpp \
    my_inheritence/ointment.cpp \
    my_inheritence/pharmacy.cpp \
    my_inheritence/pharmacymanager.cpp \
    my_inheritence/return.cpp \
    my_inheritence/safedate.cpp \
    my_inheritence/stockrecord.cpp \
    my_inheritence/storage.cpp \
    my_inheritence/supply.cpp \
    my_inheritence/syrup.cpp \
    my_inheritence/tablet.cpp \
    my_inheritence/writeoff.cpp \
    qt_classes/addproductdialog.cpp \
    qt_classes/analoguesdialog.cpp \
    qt_classes/mainwindow.cpp \
    qt_classes/operationsdialog.cpp \
    qt_classes/simpleavailabilitydialog.cpp

HEADERS += \
    Exception/FileExceptions/FileNotFoundException.h \
    Exception/FileExceptions/FileOperationException.h \
    Exception/FileExceptions/FileParseException.h \
    Exception/FileExceptions/FileWriteException.h \
    Exception/FileExceptions/SerializationException.h \
    Exception/InventoryManipExceptions/InvalidOperationException.h \
    Exception/InventoryManipExceptions/PharmacyOperationException.h \
    Exception/InventoryManipExceptions/SupplyChainException.h \
    Exception/InventoryExceptions/InsufficientQuantityException.h \
    Exception/InventoryExceptions/InventoryException.h \
    Exception/InventoryExceptions/NegativeQuantityException.h \
    Exception/InventoryExceptions/StorageOperationException.h \
    Exception/PharmacyExceptions/DuplicateProductException.h \
    Exception/PharmacyExceptions/ExpiredProductException.h \
    Exception/PharmacyExceptions/InvalidProductDataException.h \
    Exception/PharmacyExceptions/MedicalProductException.h \
    Exception/PharmacyExceptions/PharmacyException.h \
    Exception/PharmacyExceptions/ProductNotFoundException.h \
    Exception/safeinput.h \
    Files/file.h \
    Files/file_txt.h \
    file.h \
    my_binary_tree/binarytree.h \
    my_binary_tree/reverse_tree_iterator.h \
    my_binary_tree/tree_algorithms.h \
    my_binary_tree/tree_iterator.h \
    my_binary_tree/treenode.h \
    my_inheritence/filemanager.h \
    my_inheritence/inventoryoperation.h \
    my_inheritence/medicalproduct.h \
    my_inheritence/medicine.h \
    my_inheritence/ointment.h \
    my_inheritence/pharmacy.h \
    my_inheritence/pharmacymanager.h \
    my_inheritence/return.h \
    my_inheritence/safedate.h \
    my_inheritence/stockrecord.h \
    my_inheritence/storage.h \
    my_inheritence/supply.h \
    my_inheritence/syrup.h \
    my_inheritence/tablet.h \
    my_inheritence/writeoff.h \
    qt_classes/addproductdialog.h \
    qt_classes/analoguesdialog.h \
    qt_classes/mainwindow.h \
    qt_classes/operationsdialog.h \
    qt_classes/simpleavailabilitydialog.h

FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
