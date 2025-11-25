#include "file_txt.h"
#include "pharmacy.h"
#include "inventoryoperation.h"
#include "stockrecord.h"
#include <iostream>
#include <typeinfo>
#include "Exception/FileExceptions/FileWriteException.h"
#include "Exception/FileExceptions/FileNotFoundException.h"
#include "Exception/FileExceptions/FileParseException.h"
#include "Exception/FileExceptions/SerializationException.h"

template<class T>
File_text<T>::File_text(const std::string& name) : File(name) {}

template<class T>
File_text<T>::~File_text()
{
    if(file_i.is_open())
        file_i.close();
    if(file_o.is_open())
        file_o.close();
}

template<class T>
bool File_text<T>::Open_file_in()
{
    file_i.open(file_name, std::ios::in);
    return file_i.is_open();
}

template<class T>
bool File_text<T>::Open_file_out()
{
    // ИЗМЕНИТЬ: использовать append mode для добавления в конец
    file_o.open(file_name, std::ios::out | std::ios::app);
    return file_o.is_open();
}

template<class T>
bool File_text<T>::Open_file_trunc()
{
    // НОВЫЙ МЕТОД: для полной перезаписи файла
    file_o.open(file_name, std::ios::out | std::ios::trunc);
    return file_o.is_open();
}

template<class T>
void File_text<T>::Remote()
{
    if(file_i.is_open())
    {
        file_i.seekg(0, std::ios::beg);
        file_i.clear();
    }
    if(file_o.is_open())
    {
        file_o.seekp(0, std::ios::beg);
        file_o.clear();
    }
}

template<class T>
bool File_text<T>::R_end_file()
{
    return file_i.eof();
}

template<class T>
void File_text<T>::Write_string_line(const std::string& str)
{
    if (!file_o.is_open()) {
        throw FileWriteException ("File stream for writing is not open.");
    }
    file_o << str << std::endl;
}

template<class T>
void File_text<T>::Read_string_line(std::string& str)
{
    if (!file_i.is_open())
        throw FileParseException ("File stream for reading is not open.");

    // ПРОВЕРЯЕМ КОНЕЦ ФАЙЛА ПЕРЕД ЧТЕНИЕМ
    if (file_i.eof())
        throw FileParseException("End of file reached when trying to read string.");

    std::getline(file_i, str);

    // ЕСЛИ ПРОЧИТАЛИ ПУСТУЮ СТРОКУ И ДОСТИГЛИ КОНЦА - ЭТО КОНЕЦ ФАЙЛА
    if (str.empty() && file_i.eof())
        throw FileParseException("End of file reached");

    if (file_i.fail() && !file_i.eof())
        throw FileParseException("Failed to read string line from file.");
}

template<class T>
void File_text<T>::Write_record_in_file_text(T& OBJECT)
{
    const T& obj = OBJECT;
    file_o << obj;
}

template<class T>
void File_text<T>::Read_record_in_file_text(T& OBJECT)
{
    file_i >> OBJECT;
    if (file_i.fail() && file_i.eof())
        throw SerializationException (typeid(OBJECT).name(), "End of file reached");
}

template<class T>
File_text<T>& File_text<T>::operator<<(T& obj)
{
    Write_record_in_file_text(obj);
    return *this;
}

template<class T>
File_text<T>& File_text<T>::operator>>(T& obj)
{
    Read_record_in_file_text(obj);
    return *this;
}

template<class T>
File_text<T>& File_text<T>::operator<<(const std::string& str)
{
    if (!file_o.is_open())
        throw FileWriteException("File stream for writing is not open.");
    file_o << str;
    if (file_o.fail())
        throw FileWriteException("Failed to write std::string to file.");
    return *this;
}

template<class T>
File_text<T>& File_text<T>::operator<<(const char* const str)
{
    if (!file_o.is_open()) {
        throw FileWriteException("File stream for writing is not open.");
    }
    file_o << str;
    if (file_o.fail()) {
        throw FileWriteException("Failed to write C-string to file.");
    }
    return *this;
}

// Явное инстанцирование для всех используемых типов
template class File_text<std::string>;
template class File_text<Pharmacy>;
template class File_text<InventoryOperation>;
template class File_text<StockRecord>;
