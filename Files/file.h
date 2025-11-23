#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <string>


class File
{
protected:
    std::string file_name;    // Имя файла
    std::ifstream file_i;     // Поток для чтения
    std::ofstream file_o;     // Поток для записи
    std::fstream file_stream; // Поток для чтения/записи

public:
    File(const std::string& name) : file_name(name) {}
    virtual ~File() {}
};



#endif
