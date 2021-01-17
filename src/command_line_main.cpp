#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include "include/matching.h"
#include "command_line_handler.h"
#include "basic_types.h"
#include "io.h"

namespace GTD {

    //Matching information output 
    std::string matching_header = "Dancer Peer ID,Dancer Name,Dancer Email,Dancer Amount Raised,Dancer Amount Matched,Num Unique Donations";
    auto matching_row_func = [](std::ostream& fout, const std::pair<std::string, GTD::dancer_t>& p)->std::ostream&
                                    {
                                        GTD::dancer_t d = p.second;
                                        fout << d._M_dancer_id << "," << d._M_dancer_name << "," << d._M_dancer_email << ",";
                                        fout << d._M_amt_raised << "," << d._M_amt_matched << ",";
                                        fout << d._M_donors.size();
                                        return fout;
                                    };
    //Dancer statistics ouput
    std::string statistics_header = "Type,Total Fundraised,Mean Fundraising,Median Fundraising,Number of Participants,% of Total Fundraisign,% of Total Participants";
    auto statistics_row_func = [](std::ostream& fout, const std::pair<std::string, GTD::dancer_statistics_row>& p)->std::ostream&
                                    {
                                        fout << p.first << ",";
                                        auto row = p.second;
                                        fout << std::get<0>(row) << "," << std::get<1>(row) << "," << std::get<2>(row) << ",";
                                        fout << std::get<3>(row) << "," << std::get<4>(row) << "," << std::get<5>(row);
                                        return fout;
                                    };
    //Donor information output
    std::string donor_header = "Donor Name,Donor Phone,Donor Email,Amount Donated,Amount Matched";
    auto donor_row_func = [](std::ostream& fout, const GTD::donor_t& donor)->std::ostream&
                                {
                                    fout << donor._M_donor_first_name << " " << donor._M_donor_last_name << ",";
                                    fout << donor._M_donor_phone << "," << donor._M_donor_email << ",";
                                    fout << donor._M_donation_amt << "," << donor._M_matched_amt;
                                    return fout;
                                };
    void run(int argc, char** argv)
    {
        GTD::opts ops;
        try 
        {
            ops = GTD::process_command_line_args(argc, argv);
        } catch (const std::invalid_argument& ex)
        {
            std::cout << ex.what() << std::endl;
            exit(EXIT_FAILURE);
        } catch (...)
        {
            std::cout << "Unhandled Exception" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string filename = ops._M_input_file;
        std::vector<GTD::donation_t> donations = GTD::read_donations(filename);

        //Eventually, these will be read from a file?
        GTD::matching_criterion_t mc1 = {GTD::make_donation(4500*0.6, 0), 
            GTD::make_donation(4500*0.4,0), 
            GTD::make_donation(25, 0), 
            GTD::make_donation(50, 0), 
            GTD::make_donation(25, 0),
            GTD::date_time_t("12/1/2020", "00:00:00"), 
            GTD::date_time_t("12/1/2020", "13:59:59")};
        
        GTD::matching_criterion_t mc2 = {GTD::make_donation(4500*0.6, 0), 
            GTD::make_donation(4500*0.4,0), 
            GTD::make_donation(25, 0), 
            GTD::make_donation(50, 0), 
            GTD::make_donation(25, 0),
            GTD::date_time_t("12/1/2020", "14:00:00"), 
            GTD::date_time_t("12/1/2020", "23:59:59")};
        
        std::vector<GTD::matching_criterion_t> criteria = {mc2, mc1};
        //Create matching class
        GTD::matcher m(std::move(donations), std::move(criteria));
        //Perform matching calculations
        m.perform_matching_calculations();
        //Get matching information
        auto matching_info = m.get_matching_information();
        auto dancer_matching_left = m.get_dancer_matching_money_left();
        auto general_matching_left = m.get_general_matching_money_left();
        auto donor_info = m.get_donor_information();
        auto alumni_info = m.get_alumni_donor_information();
        auto dancer_statistics = m.get_dancer_statistics();
        auto hour_statistics = m.get_hourly_statistics();
        //Output matching information
        std::string output_folder = ops._M_output_folder;
        for(auto it1 = dancer_matching_left.begin(), it2 = general_matching_left.begin(); it1 != dancer_matching_left.end(); ++it1, ++it2)
        {
            std::cout << "Dancer matching money unused during round beginning at " << static_cast<std::string>(it1->first) << ": " << it1->second << "\n";
            std::cout << "General matching money unused during round beginning at " << static_cast<std::string>(it2->first) << ": " << it2->second << "\n";
            std::cout << "\n";
        }
        GTD::write_to_csv(output_folder + "/matching.csv", matching_info.begin(), matching_info.end(), GTD::matching_header, GTD::matching_row_func);
        GTD::write_to_csv(output_folder + "/dancer_statics.csv", dancer_statistics.begin(), dancer_statistics.end(), GTD::statistics_header, GTD::statistics_row_func);
        GTD::write_to_csv(output_folder + "/donors.csv", donor_info.begin(), donor_info.end(), GTD::donor_header, GTD::donor_row_func);
    }

    }
    int main(int argc, char** argv)
    {
        GTD::run(argc, argv);
        return 0;
    }