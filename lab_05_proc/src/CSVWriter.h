#ifndef STUD_96_CSVWRITER_H
#define STUD_96_CSVWRITER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

class CSVWriter;

inline static CSVWriter& endrow(CSVWriter& file);
inline static CSVWriter& flush(CSVWriter& file);

class CSVWriter
{
    std::ofstream fs_;
    bool is_first_;
    const std::string separator_;
    const std::string escape_seq_;
    const std::string special_chars_;
public:
    CSVWriter(const std::string filename, const std::string separator = ",")
            : fs_()
            , is_first_(true)
            , separator_(separator)
            , escape_seq_("\"")
            , special_chars_("\"")
    {
        fs_.exceptions(std::ios::failbit | std::ios::badbit);
        fs_.open(filename);
    }

    ~CSVWriter()
    {
        flush();
        fs_.close();
    }

    void flush()
    {
        fs_.flush();
    }

    void endrow()
    {
        fs_ << std::endl;
        is_first_ = true;
    }

    CSVWriter& operator << ( CSVWriter& (* val)(CSVWriter&))
    {
        return val(*this);
    }

    CSVWriter& operator << (const char * val)
    {
        return write(escape(val));
    }

    CSVWriter& operator << (const std::string & val)
    {
        return write(escape(val));
    }

    template<typename T>
    CSVWriter& operator << (const T& val)
    {
        return write(val);
    }

private:
    template<typename T>
    CSVWriter& write (const T& val)
    {
        if (!is_first_)
        {
            fs_ << separator_;
        }
        else
        {
            is_first_ = false;
        }
        fs_ << val;
        return *this;
    }

    std::string escape(const std::string & val)
    {
        std::ostringstream result;
       // result << '"';
        std::string::size_type to, from = 0u, len = val.length();
        while (from < len &&
               std::string::npos != (to = val.find_first_of(special_chars_, from)))
        {
            result << val.substr(from, to - from) << escape_seq_ << val[to];
            from = to + 1;
        }
        result << val.substr(from);
        return result.str();
    }
};

inline static CSVWriter& endrow(CSVWriter& file)
{
    file.endrow();
    return file;
}

inline static CSVWriter& flush(CSVWriter& file)
{
    file.flush();
    return file;
}

#endif //STUD_96_CSVWRITER_H
