#ifndef COMMAND_LINE_HANDLER_H
#define COMMAND_LINE_HANDLER_H 1

#include <string>

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
        std::string _M_output_folder = ""; 
    };

    opts process_command_line_args(int argc, char** argv);
}

#endif