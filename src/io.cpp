#include "io.h" 
#include "basic_types.h"
#include "csvstream.hh"

namespace GTD {
    using csvrow_t = std::map<std::string, std::string>;

    #define MAP_FIND(map, o) std::find_if(map.begin(), map.end(), [](const std::pair<std::string, std::string>& p){return p.first.find(o) != std::string::npos;})

    std::vector<donation_t> read_donations(const char* filename, size_t num_donations)
    {
        try 
        {
            csvstream csvin(filename);
            csvrow_t row; 
            std::vector<donation_t> donations;
            donations.reserve(num_donations);
            while (csvin >> row) 
            {
                //std::cout << row << std::endl;
                auto date = MAP_FIND(row, "Date")->second;
                auto time = MAP_FIND(row, "Time")->second;
                date_time_t dt = date_time_t::make_date_time(date, time);

                auto donor_first_name = row.find("Donor First Name")->second;
                auto donor_last_name = row.find("Donor Last Name")->second;
                auto donor_email = row.find("Donor Email")->second;
                auto donor_phone = row.find("Donor Phone")->second;
                auto donor_card = row.find("Donor Card")->second;
                auto donor_relation = row.find("Donor Relation")->second;
                auto donation_amt = make_donation(row.find("Donation Amount")->second);
                auto dancer_name = row.find("Dancer Name")->second;
                auto dancer_email = row.find("Dancer Email")->second;
                auto dancer_id = row.find("Dancer Peer ID")->second;
                auto dancer_role = row.find("Dancer Role")->second;
                auto dancer_house = row.find("Dancer House")->second;
                auto dancer_team = row.find("Dancer Team")->second;

                donations.emplace_back(
                    dt,
                    donation_amt,
                    donor_first_name,
                    donor_last_name,
                    donor_email,
                    donor_phone,
                    donor_card,
                    donor_relation,
                    dancer_name,
                    dancer_email,
                    dancer_house,
                    dancer_team,
                    dancer_role,
                    dancer_id
                );
            }
            return donations;
        } catch (const csvstream_exception& ex)
        {
            std::cout << "Error: " << filename << " not found. Program terminated." << std::endl;
            exit(EXIT_FAILURE);
        } catch (...)
        {
            std::cout << "Unhandled Exception" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

