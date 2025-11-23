#ifndef INC_5_LAB_FILE_TEXT_H
#define INC_5_LAB_FILE_TEXT_H

#include "file.h"

template<class T>
class File_text : public File
{
public:
    File_text(const std::string& name);
    ~File_text();

    bool Open_file_in();
    bool Open_file_out();
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
    //File_text<T>& operator>>(std::string& str);
};


#endif //INC_5_LAB_FILE_TEXT_H
