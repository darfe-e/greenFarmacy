QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Exception/safeinput.cpp \
    addproductdialog.cpp \
    analoguesdialog.cpp \
    availabilitydialog.cpp \
    filemanager.cpp \
    inventoryoperation.cpp \
    main.cpp \
    mainwindow.cpp \
    medicalproduct.cpp \
    medicine.cpp \
    ointment.cpp \
    pharmacy.cpp \
    pharmacymanager.cpp \
    return.cpp \
    returnsdialog.cpp \
    safedate.cpp \
    storage.cpp \
    suppliesdialog.cpp \
    supply.cpp \
    syrup.cpp \
    tablet.cpp \
    writeoff.cpp \
    writeoffsdialog.cpp

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
    Exception/PharmacyExceptions/InvalidPriceException.h \
    Exception/PharmacyExceptions/InvalidProductDataException.h \
    Exception/PharmacyExceptions/InvalidProductIdException.h \
    Exception/PharmacyExceptions/MedicalProductException.h \
    Exception/PharmacyExceptions/PharmacyException.h \
    Exception/PharmacyExceptions/ProductNotFoundException.h \
    Exception/safeinput.h \
    addproductdialog.h \
    analoguesdialog.h \
    availabilitydialog.h \
    filemanager.h \
    inventoryoperation.h \
    mainwindow.h \
    medicalproduct.h \
    medicine.h \
    ointment.h \
    pharmacy.h \
    pharmacymanager.h \
    return.h \
    returnsdialog.h \
    safedate.h \
    storage.h \
    suppliesdialog.h \
    supply.h \
    syrup.h \
    tablet.h \
    writeoff.h \
    writeoffsdialog.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
