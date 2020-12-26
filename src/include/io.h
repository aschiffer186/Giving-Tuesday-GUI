#ifndef EXCEL_IO_H
#define EXCEL_IO_H

//For now, only read from .csv to make things easier; will add .xlsx support in the future 

#include "basic_types.h"
#include <vector>
#include <ostream> 
#include <fstream>
#include <map>
#include <unordered_map>

namespace GTD {
    //Reads in the donations from a list of donations and compiles them into a vector.
    //
    //@param filename the file containing the donations 
    //@param num_donations optional paramater, the number of donations (knowing this will save
    //                     time and memory) 
    std::vector<donation_t> read_donations(const std::string& filename, size_t num_donations = 0);


    template<typename _KTp, typename _VTp>
    std::ostream& operator<<(std::ostream& os, const std::map<_KTp, _VTp> & m)
    {
        os << "{";
        for (const auto& p : m)
        {
            os <<"{" << p.first << ", " << p.second << "}";
        }
        os << "}";
        return os;
    }

    template<typename _KTp, typename _VTp>
    std::ostream& operator<<(std::ostream& os, const std::unordered_map<_KTp, _VTp> & m)
    {
        os << "{";
        for (const auto& p : m)
        {
            os <<"{" << p.first << ", " << p.second << "}";
        }
        os << "}";
        return os;
    }


    template<typename _IterTp, typename _FuncTp>
    void write_to_file(const char* filename, _IterTp begin, _IterTp end, const std::string& header_row, _FuncTp print_row)
    {
        std::ofstream fout(filename);
        fout << header_row << "\n";
        while(begin != end) 
        {
            print_row(fout, *begin++);
            fout << "\n";
        }
        fout.close();
    }

}

#endif