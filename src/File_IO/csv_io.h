#ifndef CSV_IO_H
#define CSV_IO_H 

#include <string>
#include <vector>
#include "Analysis/basic_types.h"
#include "Analysis/matching.h"
#include <numeric>
#include <fstream>

namespace Fundraising::IO
{
    //Lamnda functions for .csv output
    const static std::string matching_header = "Dancer Peer ID,Dancer Name,Dancer Email,Dancer Amount Raised,Dancer Amount Matched,Num Unique Donations";;
    auto matching_row_func = [](std::ostream& fout, const std::pair<std::string, Analysis::dancer_t>& p)->std::ostream&
                                    {
                                        Analysis::dancer_t d = p.second;
                                        fout << d._M_dancer_id << "," << d._M_dancer_name << "," << d._M_dancer_email << ",";
                                        fout << d._M_amt_raised << "," << d._M_amt_matched << ",";
                                        fout << d._M_donors.size();
                                        return fout;
                                    };
    //Dancer statistics ouput
    const static std::string statistics_header = "Type,Total Fundraised,Mean Fundraising,Median Fundraising,% of Total Fundraising,Number of Participants,% of Total Participants";
    auto statistics_row_func = [](std::ostream& fout, const std::pair<std::string, Analysis::dancer_statistics_row>& p)->std::ostream&
                                    {
                                        fout << p.first << ",";
                                        auto row = p.second;
                                        fout << std::get<0>(row) << "," << std::get<1>(row) << "," << std::get<2>(row) << ",";
                                        fout << std::get<3>(row) << "," << std::get<4>(row) << "," << std::get<5>(row);
                                        return fout;
                                    };
    //Donor information output
    const static std::string donor_header = "Donor Name,Donor Phone,Donor Email,Amount Donated,Amount Matched";
    auto donor_row_func = [](std::ostream& fout, const Analysis::donor_t& donor)->std::ostream&
                                {
                                    fout << donor._M_donor_first_name << " " << donor._M_donor_last_name << ",";
                                    fout << donor._M_donor_phone << "," << donor._M_donor_email << ",";
                                    fout << donor._M_donation_amt << "," << donor._M_matched_amt;
                                    return fout;
                                };
    
    const static std::string alumni_donor_header = "Donor Name,Donor Phone,Donor Email,Amount Donated,Amount Matched,Num Donated To";
    auto alumni_row_func = [](std::ostream& fout, const Analysis::donor_t& donor)->std::ostream&
                            {
                                fout << donor._M_donor_first_name << " " << donor._M_donor_last_name << ",";
                                fout << donor._M_donor_phone << "," << donor._M_donor_email << ",";
                                fout << donor._M_donation_amt << "," << donor._M_matched_amt << ",";
                                size_t num_dancers = std::accumulate(donor._M_dancer_ids.begin(), donor._M_dancer_ids.end(), 0,
                                                                    [](size_t i, const std::pair<std::string, std::unordered_set<std::string>>& pair)
                                                                    {
                                                                        return i + pair.second.size();
                                                                    });
                                fout << std::to_string(num_dancers);
                                return fout;
                            };
    
    const static std::string alumni_statistics_header = "DMUM,Dancer,Leadership";
    auto alumni_statistics_row = [](std::ostream& fout, Analysis::donor_t donor)->std::ostream&
                                {
                                    fout << std::to_string(donor._M_dancer_ids["DMUM"].size()) << ",";
                                    fout << std::to_string(donor._M_dancer_ids["Dancer"].size()) << ",";
                                    fout << std::to_string(donor._M_dancer_ids["Leadership"].size());
                                    return fout;
                                };

    const static std::string hourly_statistics_header = "Hour,Hourly fundraising,mean donation size,median donation size,num donors,num unique donors,number of alumni donors,number of unique alumni donors";
    auto hour_statistics_func = [](std::ostream& fout,const auto& p)->std::ostream&
                                {
                                    auto row = p.second;
                                    Analysis::date_time_t dt = p.first;
                                    std::get<1>(dt._M_time) = 0;
                                    std::get<2>(dt._M_time) = 0;
                                    fout << static_cast<std::string>(dt) << ",";
                                    fout << std::get<0>(row) << "," << std::get<1>(row) << "," << std::get<2>(row)  << ",";
                                    fout << std::get<3>(row) << "," << std::get<4>(row) << "," << std::get<5>(row)  << ",";
                                    fout << std::get<6>(row);
                                    return fout;
                                };

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