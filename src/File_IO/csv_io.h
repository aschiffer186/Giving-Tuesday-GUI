#ifndef CSV_IO_H
#define CSV_IO_H 

#include <string>
#include <vector>
#include "Analysis/basic_types.h"
#include <fstream>

namespace Fundraising::IO
{
    std::vector<Analysis::donation_t> read_csv_donations(const std::string& filename, size_t num_donations = 0);

    template<typename _IterTp, typename _FuncTp>
    void write_to_csv(const std::string& filename, _IterTp begin, _IterTp end, const std::string& header_row, _FuncTp print_row)
    {
        std::ofstream fout(filename.c_str());
        fout << header_row << "\n";
        while(begin != end) 
        {
            print_row(fout,*begin++);
            fout << "\n";
        }
        fout.close();
    }
} // namespace Fundraisng::IO


#endif