#include "command_line_handler.h"
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
}