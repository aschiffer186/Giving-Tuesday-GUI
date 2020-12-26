#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include "matching.h"
#include "command_line_handler.h"
#include "basic_types.h"
#include "io.h"

int main(int argc, char** argv)
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
    const char* filename = ops._M_input_file;
    std::vector<GTD::donation_t> donations = GTD::read_donations(std::string(filename));

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

    GTD::matcher m(std::move(donations), std::move(criteria));
    m.perform_matching_calculations();

}