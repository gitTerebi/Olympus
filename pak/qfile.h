#ifndef QFILE_H
#define QFILE_H

#include <fstream>

class QFile : private std::ifstream {
public:
    QFile(const std::string& filename);

    void reset();

    bool isReadable();
    bool atEnd();

    using std::ifstream::close;
    int64_t size() const;
    int64_t pos();

    void seek(const int64_t pos);

    int64_t read(char* const data, const int64_t maxSize);

    bool getChar(char* const data);
private:
    std::streampos mSize;
};

#endif // QFILE_H
