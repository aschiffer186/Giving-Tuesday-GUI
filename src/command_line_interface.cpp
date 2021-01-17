#include "command_line_interface.h"
#include "matching.h"
#include "io.h"
#include <getopt.h>
#include <stdexcept>
#include <iostream>

option long_options[] = 
{
    {"input", required_argument, nullptr, 'i'},
    {"output", required_argument, nullptr, 'o'},
    {"num_donations", required_argument, nullptr, 'n'}, 
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0}
};

namespace GTD
{
    opts process_command_line_args(int argc, char** argv)
    {
        opts ops;
        bool input_seen = false;
        bool output_seen = false;
        bool num_donations_seen = false;

        int choice = 0;
        long long num_d;
        while ((choice = getopt_long(argc, argv, "i:o:n:h", long_options, nullptr)) != -1) 
        {
            switch(choice)
            {
                case 'i':
                    if (input_seen)
                        throw std::invalid_argument("May only define input file once");
                    input_seen = true;
                    ops._M_input_file = optarg;
                    break;
                case 'o':
                    if (output_seen)
                        throw std::invalid_argument("May only define output directory once");
                    output_seen = true;
                    ops._M_output_folder = optarg;
                    break;
                case 'n':
                    if (num_donations_seen)
                        throw std::invalid_argument("May only define number of donations once");
                    num_donations_seen = true;
                    num_d = std::atoll(optarg);
                    if (num_d < 0)
                        throw std::invalid_argument("Number of donations must be non-negative");
                    ops._M_num_donations = static_cast<size_t>(num_d);
                    break;
                case 'h': 
                    std::cout << 
                    " --input [file name] or -i [filename] \n"
                    "   Specify the input file name. May be a .csv or .xlsx file\n"
                    "--output [folder name] or -o [folder name] \n"
                    "   (Optional) Specify the output directory name\n"
                    "--num-donations [amount] or -n [amount]\n"
                    "   (Optional) Specify the number of donations. Optional but doing so may speed up the proprgram."
                    ;
                    std::exit(EXIT_SUCCESS);
                    break;
                default: 
                    std::cout << "Invalid argument\n";
                    std::cout << 
                    " --input [file name] or -i [filename] \n"
                    "   Specify the input file name. May be a .csv or .xlsx file\n"
                    "--output [folder name] or -o [folder name] \n"
                    "   (Optional) Specify the output directory name\n"
                    "--num-donations [amount] or -n [amount]\n"
                    "   (Optional) Specify the number of donations. Optional but doing so may speed up the program."
                    ;
                    std::exit(EXIT_SUCCESS);
                    break;
            }
        }
        if(!input_seen)
            throw std::invalid_argument("Must specify input filename");
        if (!output_seen)
            ops._M_output_folder = "output";
        if(!num_donations_seen)
            ops._M_num_donations = 0;
        return ops;
    }

    void command_line_run(int argc, char** argv)
    {
        opts ops;
        try 
        {
            ops = process_command_line_args(argc, argv);
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
        std::vector<donation_t> donations = read_donations(filename);

        //Eventually, these will be read from a file?
        matching_criterion_t mc1 = {make_donation(4500*0.6, 0), 
            make_donation(4500*0.4,0), 
            make_donation(25, 0), 
            make_donation(50, 0), 
            make_donation(25, 0),
            date_time_t("12/1/2020", "00:00:00"), 
            date_time_t("12/1/2020", "13:59:59")};
        
        matching_criterion_t mc2 = {make_donation(4500*0.6, 0), 
            make_donation(4500*0.4,0), 
            make_donation(25, 0), 
            make_donation(50, 0), 
            make_donation(25, 0),
            date_time_t("12/1/2020", "14:00:00"), 
            date_time_t("12/1/2020", "23:59:59")};
        
        std::vector<matching_criterion_t> criteria = {mc2, mc1};
        //Create matching class
        matcher m(std::move(donations), std::move(criteria));
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
        write_to_csv(output_folder + "/matching.csv", matching_info.begin(), matching_info.end(), matching_header, matching_row_func);
        write_to_csv(output_folder + "/dancer_statics.csv", dancer_statistics.begin(), dancer_statistics.end(), statistics_header, statistics_row_func);
        write_to_csv(output_folder + "/donors.csv", donor_info.begin(), donor_info.end(), donor_header, donor_row_func);
        write_to_csv(output_folder + "/alumni_donors.csv", alumni_info.begin(), alumni_info.end(), alumni_donor_header, alumni_row_func);
        write_to_csv(output_folder + "/alumni_statistics.csv", alumni_info.begin(), alumni_info.end(), alumni_statistics_header, alumni_statistics_row);
        write_to_csv(output_folder + "/hourly_statistics.csv", hour_statistics.begin(), hour_statistics.end(), hourly_statistics_header, hour_statistics_row);
    }
}