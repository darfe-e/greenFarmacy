#ifndef INC_5_LAB_FILE_TEXT_H
#define INC_5_LAB_FILE_TEXT_H

#include "file.h"
#include <fstream>
#include <string>
#include <stdexcept>

template<class T>
class File_text : public File
{
public:
    File_text(const std::string& name);
    ~File_text();

    bool Open_file_in();
    bool Open_file_out(); // для добавления в конец
    bool Open_file_trunc(); // ДОБАВИТЬ: для полной перезаписи
    void Remote();
    void Write_record_in_file_text(T& OBJECT);
    void Read_record_in_file_text(T& OBJECT);
    bool R_end_file();
    void Read_string_line(std::string& str);
    void Write_string_line(const std::string& str);

    // Перегруженные операторы для удобства
    File_text<T>& operator<<(T& obj);
    File_text<T>& operator>>(T& obj);
    File_text<T>& operator<<(const std::string& str);
    File_text<T>& operator<<(const char* const str);

    void Close_file_in() { if(file_i.is_open()) { file_i.close(); } }

    void Close_file_o()
    {
        if(file_o.is_open())
        {
            file_o.flush();
            file_o.close();
        }
    }

    void Close_file_out() { if (file_o.is_open()) { file_o.close(); } }

    void Flush() { if(file_o.is_open()) { file_o.flush(); } }

    void Reset_file()
    {
        if(file_i.is_open())
        {
            file_i.clear();
            file_i.seekg(0, std::ios::beg);
        }
    }
};

#endif //INC_5_LAB_FILE_TEXT_H
