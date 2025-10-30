#ifndef FILEPARSEEXCEPTION_H
#define FILEPARSEEXCEPTION_H

#include "FileOperationException.h"

class FileParseException : public FileOperationException
{
private:
    std::string filename;
    int lineNumber;
    std::string lineContent;

public:
    FileParseException(const std::string& file, int line, const std::string& content, const std::string& details = "")
        : FileOperationException("Parse error in file '" + file + "' at line " +
                                 std::to_string(line) + ": " + details),
        filename(file), lineNumber(line), lineContent(content) {}

    std::string getErrorCode() const override { return "FILE_PARSE_001"; }
    std::string getFilename() const { return filename; }
    int getLineNumber() const { return lineNumber; }
    std::string getLineContent() const { return lineContent; }
};

#endif // FILEPARSEEXCEPTION_H
