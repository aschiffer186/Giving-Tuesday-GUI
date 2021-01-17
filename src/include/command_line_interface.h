#ifndef COMMAND_LINE_HANDLER_H
#define COMMAND_LINE_HANDLER_H 1

#include <string>
#include <numeric>
#include <ostream>
#include "basic_types.h"
#include "matching.h"

namespace GTD
{
    //Handle ommand line options 
    //Comand line options are 
    //  --input (-i) the input file name (required)
    //  --output (-o) the output directory (optional)
    //  --num-donations (-n) the number of donations (optional)
    struct opts
    {
        size_t _M_num_donations = 0; 
        std::string _M_input_file;
        std::string _M_output_folder = "output"; 
    };

    opts process_command_line_args(int argc, char** argv);

    //Handle command line output
    const static std::string matching_header = "Dancer Peer ID,Dancer Name,Dancer Email,Dancer Amount Raised,Dancer Amount Matched,Num Unique Donations";;
    auto matching_row_func = [](std::ostream& fout, const std::pair<std::string, GTD::dancer_t>& p)->std::ostream&
                                    {
                                        GTD::dancer_t d = p.second;
                                        fout << d._M_dancer_id << "," << d._M_dancer_name << "," << d._M_dancer_email << ",";
                                        fout << d._M_amt_raised << "," << d._M_amt_matched << ",";
                                        fout << d._M_donors.size();
                                        return fout;
                                    };
    //Dancer statistics ouput
    const static std::string statistics_header = "Type,Total Fundraised,Mean Fundraising,Median Fundraising,Number of Participants,% of Total Fundraisign,% of Total Participants";
    auto statistics_row_func = [](std::ostream& fout, const std::pair<std::string, GTD::dancer_statistics_row>& p)->std::ostream&
                                    {
                                        fout << p.first << ",";
                                        auto row = p.second;
                                        fout << std::get<0>(row) << "," << std::get<1>(row) << "," << std::get<2>(row) << ",";
                                        fout << std::get<3>(row) << "," << std::get<4>(row) << "," << std::get<5>(row);
                                        return fout;
                                    };
    //Donor information output
    const static std::string donor_header = "Donor Name,Donor Phone,Donor Email,Amount Donated,Amount Matched";
    auto donor_row_func = [](std::ostream& fout, const GTD::donor_t& donor)->std::ostream&
                                {
                                    fout << donor._M_donor_first_name << " " << donor._M_donor_last_name << ",";
                                    fout << donor._M_donor_phone << "," << donor._M_donor_email << ",";
                                    fout << donor._M_donation_amt << "," << donor._M_matched_amt;
                                    return fout;
                                };
    
    const static std::string alumni_donor_header = "Donor Name,Donor Phone,Donor Email,Amount Donated,Amount Matched,Num Donated To";
    auto alumni_row_func = [](std::ostream& fout, const GTD::donor_t& donor)->std::ostream&
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
    auto alumni_statistics_row = [](std::ostream& fout, GTD::donor_t donor)->std::ostream&
                                {
                                    fout << std::to_string(donor._M_dancer_ids["DMUM"].size()) << ",";
                                    fout << std::to_string(donor._M_dancer_ids["Dancer"].size()) << ",";
                                    fout << std::to_string(donor._M_dancer_ids["Leadership"].size());
                                    return fout;
                                };

    void command_line_run(int argc, char** argv);
}

#endif