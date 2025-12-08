#include "file_txt.h"
#include "my_inheritence/pharmacy.h"
#include "my_inheritence/inventoryoperation.h"
#include "my_inheritence/stockrecord.h"
#include <iostream>
#include <typeinfo>
#include "Exception/FileExceptions/FileWriteException.h"
#include "Exception/FileExceptions/FileNotFoundException.h"
#include "Exception/FileExceptions/FileParseException.h"
#include "Exception/FileExceptions/SerializationException.h"

template<class T>
File_text<T>::File_text(const std::string& name) : File(name)               // Конструктор с именем файла
{
}

template<class T>
File_text<T>::~File_text()                                                  // Деструктор
{
    if(file_i.is_open())                                                    // Закрытие входного файла, если открыт
        file_i.close();
    if(file_o.is_open())                                                    // Закрытие выходного файла, если открыт
        file_o.close();
}

template<class T>
bool File_text<T>::Open_file_in()                                           // Открытие файла для чтения
{
    file_i.open(file_name, std::ios::in);                                   // Открытие в режиме чтения
    return file_i.is_open();                                                // Возврат статуса открытия
}

template<class T>
bool File_text<T>::Open_file_out()                                          // Открытие файла для записи (добавление)
{
    file_o.open(file_name, std::ios::out | std::ios::app);                  // Открытие в режиме записи и добавления
    return file_o.is_open();                                                // Возврат статуса открытия
}

template<class T>
bool File_text<T>::Open_file_trunc()                                        // Открытие файла для полной перезаписи
{
    file_o.open(file_name, std::ios::out | std::ios::trunc);                // Открытие в режиме записи с очисткой файла
    return file_o.is_open();                                                // Возврат статуса открытия
}

template<class T>
void File_text<T>::Remote()                                                 // Сброс позиции чтения/записи в начало
{
    if(file_i.is_open())                                                    // Если входной файл открыт
    {
        file_i.seekg(0, std::ios::beg);                                     // Переход к началу файла для чтения
        file_i.clear();                                                     // Сброс флагов ошибок
    }
    if(file_o.is_open())                                                    // Если выходной файл открыт
    {
        file_o.seekp(0, std::ios::beg);                                     // Переход к началу файла для записи
        file_o.clear();                                                     // Сброс флагов ошибок
    }
}

template<class T>
bool File_text<T>::R_end_file()                                             // Проверка достижения конца файла
{
    return file_i.eof();                                                    // Возврат флага конца файла
}

template<class T>
void File_text<T>::Write_string_line(const std::string& str)                // Запись строки в файл
{
    if (!file_o.is_open())                                                  // Проверка открытия файла для записи
        throw FileWriteException ("File stream for writing is not open.");

    file_o << str << std::endl;                                             // Запись строки с переводом строки
}

template<class T>
void File_text<T>::Read_string_line(std::string& str)                       // Чтение строки из файла
{
    if (!file_i.is_open())                                                  // Проверка открытия файла для чтения
        throw FileParseException ("File stream for reading is not open.");

    if (file_i.eof())                                                       // Проверка конца файла перед чтением
        throw FileParseException("End of file reached when trying to read string.");

    std::getline(file_i, str);                                              // Чтение строки из файла

    if (str.empty() && file_i.eof())                                        // Если прочитана пустая строка и конец файла
        throw FileParseException("End of file reached");

    if (file_i.fail() && !file_i.eof())                                     // Если произошла ошибка чтения
        throw FileParseException("Failed to read string line from file.");
}

template<class T>
void File_text<T>::Write_record_in_file_text(T& OBJECT)                     // Запись объекта в файл
{
    const T& obj = OBJECT;                                                  // Создание константной ссылки на объект
    file_o << obj;                                                          // Использование оператора вывода для объекта
}

template<class T>
void File_text<T>::Read_record_in_file_text(T& OBJECT)                      // Чтение объекта из файла
{
    file_i >> OBJECT;                                                       // Использование оператора ввода для объекта
    if (file_i.fail() && file_i.eof())                                      // Проверка ошибок чтения
        throw SerializationException (typeid(OBJECT).name(), "End of file reached");
}

template<class T>
File_text<T>& File_text<T>::operator<<(T& obj)                              // Оператор вывода для объектов
{
    Write_record_in_file_text(obj);                                         // Запись объекта в файл
    return *this;                                                           // Возврат ссылки на текущий объект
}

template<class T>
File_text<T>& File_text<T>::operator>>(T& obj)                              // Оператор ввода для объектов
{
    Read_record_in_file_text(obj);                                          // Чтение объекта из файла
    return *this;                                                           // Возврат ссылки на текущий объект
}

template<class T>
File_text<T>& File_text<T>::operator<<(const std::string& str)              // Оператор вывода для строк std::string
{
    if (!file_o.is_open())                                                  // Проверка открытия файла для записи
        throw FileWriteException("File stream for writing is not open.");

    file_o << str;                                                          // Запись строки
    if (file_o.fail())                                                      // Проверка ошибок записи
        throw FileWriteException("Failed to write std::string to file.");

    return *this;                                                           // Возврат ссылки на текущий объект
}

template<class T>
File_text<T>& File_text<T>::operator<<(const char* const str)               // Оператор вывода для C-строк
{
    if (!file_o.is_open())                                                  // Проверка открытия файла для записи
        throw FileWriteException("File stream for writing is not open.");

    file_o << str;                                                          // Запись C-строки
    if (file_o.fail())                                                      // Проверка ошибок записи
        throw FileWriteException("Failed to write C-string to file.");

    return *this;                                                           // Возврат ссылки на текущий объект
}

template class File_text<std::string>;                                      // Явное инстанцирование для std::string
template class File_text<Pharmacy>;                                         // Явное инстанцирование для Pharmacy
template class File_text<InventoryOperation>;                               // Явное инстанцирование для InventoryOperation
template class File_text<StockRecord>;                                      // Явное инстанцирование для StockRecord
