#include "command_line_interface.h"
#include "Analysis/criterion_parser.h"
#include "Analysis/matching.h"
#include "File_IO/csv_io.h"
#include <getopt.h>
#include <stdexcept>
#include <iostream>

option long_options[] = 
{
    {"input", required_argument, nullptr, 'i'},
    {"output", required_argument, nullptr, 'o'},
    {"num_donations", required_argument, nullptr, 'n'}, 
    {"criteria", required_argument, nullptr, 'c'},
    {"help", no_argument, nullptr, 'h'},
    {nullptr, 0, nullptr, 0}
};

namespace Fundraising::Command_Line
{
    opts process_command_line_args(int argc, char** argv)
    {
        opts ops;
        bool input_seen = false;
        bool output_seen = false;
        bool num_donations_seen = false;
        bool criteia_file_seen = false;

        int choice = 0;
        long long num_d;
        while ((choice = getopt_long(argc, argv, "i:o:n:c:h", long_options, nullptr)) != -1) 
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
                case 'c':
                    if(criteia_file_seen)
                        throw std::invalid_argument("May only specify criteria file once");
                    criteia_file_seen = true;
                    ops._M_criterion_input_file = optarg;
                    break;
                case 'h': 
                    std::cout << 
                    " --input [filename] or -i [filename] \n"
                    "   Specify the input file name. May be a .csv or .xlsx file\n"
                    "--output [folder name] or -o [folder name] \n"
                    "   (Optional) Specify the output directory name\n"
                    "--num-donations [amount] or -n [amount]\n"
                    "   (Optional) Specify the number of donations. Optional but doing so may speed up the proprgram.\n"
                    "--criteria [filenname] or -c [filename]\n"
                    "   (Optional) Specify a filename with matching criteria"
                    ;
                    std::exit(EXIT_SUCCESS);
                    break;
                default: 
                    std::cerr << "Invalid argument\n";
                    std::cout << 
                    " --input [file name] or -i [filename] \n"
                    "   Specify the input file name. May be a .csv or .xlsx file\n"
                    "--output [folder name] or -o [folder name] \n"
                    "   (Optional) Specify the output directory name\n"
                    "--num-donations [amount] or -n [amount]\n"
                    "   (Optional) Specify the number of donations. Optional but doing so may speed up the program."
                    "--criteria [filenname] or -c [filename]\n"
                    "   (Optional) Specify a filename with matching criteria"
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
        if(!criteia_file_seen)
            ops._M_criterion_input_file = "";
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
            std::cerr << ex.what() << std::endl;
            exit(EXIT_FAILURE);
        } catch (...)
        {
            std::cerr << "Unhandled Exception" << std::endl;
            exit(EXIT_FAILURE);
        }
        std::string filename = ops._M_input_file;
        std::vector<Analysis::donation_t> donations = IO::read_csv_donations(filename);

        //Eventually, these will be read from a file?
        /*
        Analysis::matching_criterion_t mc1 = {
            Analysis::make_donation(4500*0.6, 0), 
            Analysis::make_donation(4500*0.4,0), 
            Analysis::make_donation(25, 0), 
            Analysis::make_donation(50, 0), 
            Analysis::make_donation(25, 0),
            Analysis::date_time_t("12/1/2020", "00:00:00"), 
            Analysis::date_time_t("12/1/2020", "13:59:59")};
        
        Analysis::matching_criterion_t mc2 = {
            Analysis::make_donation(4500*0.6, 0), 
            Analysis::make_donation(4500*0.4,0), 
            Analysis::make_donation(25, 0), 
            Analysis::make_donation(50, 0), 
            Analysis::make_donation(25, 0),
            Analysis::date_time_t("12/1/2020", "14:00:00"), 
            Analysis::date_time_t("12/1/2020", "23:59:59")};
        */
        
        std::vector<Analysis::matching_criterion_t> criteria;
        if (!ops._M_criterion_input_file.empty()) 
        {
            std::ifstream criteria_in(ops._M_criterion_input_file.c_str());
            if(!criteria_in.is_open())
            {
                std::cerr << "Error opening file" << std::endl;
                exit(EXIT_FAILURE);
            }
            Analysis::parser p(criteria_in);
            try {
            criteria = p.parse_criteria();
            } catch (const std::runtime_error& ex) {
                std::cerr << ex.what() << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        std::sort(criteria.begin(), criteria.end(), [](const Analysis::matching_criterion_t& lhs, const Analysis::matching_criterion_t& rhs){return lhs._M_start > rhs._M_start;});
        for(const auto& c: criteria)
        {
            std::cout << static_cast<std::string>(c._M_start) << std::endl;
        }
        //Create matching class
        Analysis::matcher m(donations, criteria);
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
        IO::write_to_csv(output_folder + "/matching.csv", matching_info.begin(), matching_info.end(), IO::matching_header, IO::matching_row_func);
        IO::write_to_csv(output_folder + "/dancer_statics.csv", dancer_statistics.begin(), dancer_statistics.end(), IO::statistics_header, IO::statistics_row_func);
        IO::write_to_csv(output_folder + "/donors.csv", donor_info.begin(), donor_info.end(), IO::donor_header, IO::donor_row_func);
        IO::write_to_csv(output_folder + "/alumni_donors.csv", alumni_info.begin(), alumni_info.end(), IO::alumni_donor_header, IO::alumni_row_func);
        IO::write_to_csv(output_folder + "/alumni_statistics.csv", alumni_info.begin(), alumni_info.end(), IO::alumni_statistics_header, IO::alumni_statistics_row);
        IO::write_to_csv(output_folder + "/hourly_statistics.csv", hour_statistics.begin(), hour_statistics.end(), IO::hourly_statistics_header, IO::hour_statistics_func);
    }
}