#include "criterion_parser.h"
#include "basic_types.h"
#include <string>
#include <algorithm>
#include <stdexcept>
#include <regex>
#include <iostream>

namespace Fundraising::Analysis
{
    parser::parser(std:: istream& in)
        : _M_in(in)
    {
        seenBegin = false;
        seenEnd = true;
        seenMaxPerDancer = false;
        seenMaxPerDonor = false;
        seenMaxPerDonation = false;
        seenGenMatching = false;
        seenDancerMatching = false;
    }

    //NOT how to write a parser 
    //This is a very bad way to approximate a DFSM that drives 
    //most parsers. But our input grammar is very simple 
    //so it's ok.
    std::vector<matching_criterion_t> parser::parse_criteria()
    {
        std::vector<matching_criterion_t> criteria;
        std::string line;
        matching_criterion_t criterion;
        date_time_t start_ts;
        date_time_t end_ts;
        while(std::getline(_M_in, line))
        {
            if(find_ignore_case(line, "BEGIN CRITERION"))
            {
                if(!seenEnd)
                    throw std::runtime_error("Invalid matching criteria file 1");
                seenBegin = true;
                seenEnd = false;
            }

            //Parse start date of matching criterion
            else if(find_ignore_case(line, "START DATE"))
            {
                if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 2");
                std::regex e("\\d{2,4}(-|/)\\d{2}(-|/)\\d{2,4}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 3");
                std::tuple<int,int,int> end_date = parse_date(it->str());
                std::tuple<int,int,int> start_date = parse_date(it->str());
                start_ts._M_date = start_date;
            }

            //Parse end date of matching criterion
            else if (find_ignore_case(line, "END DATE"))
            {
                if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 4");
                std::regex e("\\d{2,4}(-|/)\\d{2}(-|/)\\d{2,4}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 5");
                std::tuple<int,short,short> end_date = parse_date(it->str());
                end_ts._M_date = end_date;
            }

            //Parse start time of matching criterion 
            else if(find_ignore_case(line, "START TIME"))
            {
                if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 6");
                std::regex e("\\d{2}:\\d{2}:\\d{2}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 7");
                auto start_time = parse_time(it->str());
                start_ts._M_time = start_time;
            }

            else if(find_ignore_case(line, "END TIME"))
            {
                if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 8");
                std::regex e("\\d{2}:\\d{2}:\\d{2}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 9");
                auto end_time = parse_time(it->str());
                end_ts._M_time = end_time;
            }

            //Parse dancer amount 
            else if(find_ignore_case(line, "DANCER MATCHING AMOUNT"))
            {
                if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 10");
                std::regex e("\\d+.\\d{0,2}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 11");
                std::string amt = it->str();
                criterion._M_dancer_amt = make_donation(amt); 
                seenDancerMatching = true; 
            }

            else if(find_ignore_case(line, "GENERAL MATCHING AMOUNT"))
            {
                 if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 12");
                std::regex e("\\d+.\\d{0,2}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 13");
                std::string amt = it->str();
                criterion._M_general_amt = make_donation(amt);
                seenGenMatching = true;  
            }

            else if(find_ignore_case(line, "MAX PER DONATION"))
            {
                 if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 14");
                std::regex e("\\d+.\\d{0,2}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 15");
                std::string amt = it->str();
                criterion._M_max_per_donation = make_donation(amt);
                seenMaxPerDonation = true;
            }

            else if(find_ignore_case(line, "MAX PER DONOR"))
            {
                 if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 16");
                std::regex e("\\d+.\\d{0,2}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 17");
                std::string amt = it->str();
                criterion._M_max_per_donor = make_donation(amt);
                seenMaxPerDonor = true;
            }

            else if(find_ignore_case(line, "MAX PER DANCER"))
            {
                 if(!check_file_validity())
                    throw std::runtime_error("Invalid matching criteria file 18");
                std::regex e("\\d+.\\d{0,2}");
                auto it = std::sregex_iterator(line.begin(), line.end(), e);
                std::sregex_iterator rend;
                if(it == rend)
                    throw std::runtime_error("Invalid matching criteria file 19");
                std::string amt = it->str();
                criterion._M_max_per_person = make_donation(amt);
                seenMaxPerDancer = true;
            }

            else if(find_ignore_case(line, "END CRITERION"))
            {
                if (!seenBegin)
                    throw std::runtime_error("Invalid matching criteria file 20");
                criterion._M_start = start_ts;
                criterion._M_end = end_ts;
                //If no matching amount is specified, assume no matching 
                if(!seenGenMatching)
                   criterion._M_general_amt = ZERO;
                if(!seenDancerMatching)
                    criterion._M_dancer_amt = ZERO;
                //Handle defaults
                //If no constraint is specified, assume unlimited
                if(!seenMaxPerDancer)
                    criterion._M_max_per_person = std::make_pair(std::numeric_limits<int>::max(), 0);
                if(!seenMaxPerDonation)
                    criterion._M_max_per_donation = std::make_pair(std::numeric_limits<int>::max(),0);
                if(!seenMaxPerDonor)
                    criterion._M_max_per_donor = std::make_pair(std::numeric_limits<int>::max(), 0);
                criteria.push_back(criterion);
                reset();
            }
            
        }
        return criteria;
    }

    bool parser::check_file_validity() const
    {
        return seenBegin && !seenEnd;
    }

    bool parser::find_ignore_case(const std::string& target, const std::string& goal)
    {
        auto it = std::search(target.begin(), target.end(), goal.begin(), goal.end(), [](char a, char b)
        {
            return tolower(a) == tolower(b);
        });
        return it != target.end();
    }

    std::tuple<int,short,short> parser::parse_date(const std::string& str)
    {
        if(str.length() != 10)
            throw std::runtime_error("Invalid matching criteria file 21");
        char delimiter = (str.find("-") == std::string::npos) ? '/' : '-';
        size_t first_delimiter_idx = str.find(delimiter);
        if (!(first_delimiter_idx == 2 ||  first_delimiter_idx == 4))
            throw std::runtime_error("Invalid matching criteria file 22");
        if (first_delimiter_idx == 2) //MM-DD-YYYY
        {
            short month = static_cast<short>(std::stoi(str.substr(0, first_delimiter_idx)));
            size_t second_delimiter_idx = str.find(delimiter, first_delimiter_idx + 1);
            if (second_delimiter_idx - first_delimiter_idx != 3)
                throw std::runtime_error("Invalid matching criteria file 23");
            short day = static_cast<short>(std::stoi(str.substr(first_delimiter_idx + 1, second_delimiter_idx)));
            int year = std::stoi(str.substr(second_delimiter_idx + 1));
            return std::make_tuple(year, month, day);
        } 
        else 
        {
            int year = std::stoi(str.substr(0, first_delimiter_idx));
             size_t second_delimiter_idx = str.find(delimiter, first_delimiter_idx + 1);
            if (second_delimiter_idx - first_delimiter_idx != 3)
                throw std::runtime_error("Invalid matching criteria file 24");
            short month = static_cast<short>(std::stoi(str.substr(first_delimiter_idx + 1, second_delimiter_idx)));
            short day = static_cast<short>(std::stoi(str.substr(second_delimiter_idx + 1)));
            return std::make_tuple(year, month, day);
        }
    }

    std::tuple<short,short,short> parser::parse_time(const std::string& time)
    {
        if(time.length() != 8)
            throw std::runtime_error("Invalid matching criterial file 25");
        short hour = static_cast<short>(std::stoi(time.substr(0, 2)));
        short min = static_cast<short>(std::stoi(time.substr(3,5)));
        short sec = static_cast<short>(std::stoi(time.substr(6)));
        return std::make_tuple(hour,min,sec);
    }

    void parser::reset()
    {
        seenBegin = false;
        seenEnd = true;
        seenMaxPerDancer = false;
        seenMaxPerDonor = false;
        seenMaxPerDonation = false;
        seenGenMatching = false;
        seenDancerMatching = false;
    }

    writer::writer(std::ostream& os)
    : _M_out(os)
    {

    }

    void writer::write_criteria(const std::vector<matching_criterion_t>& vec)
    {
        for(const auto& c: vec)
        {
            _M_out << "BEGIN CRITERION" << "\n";
            std::string start_ts = static_cast<std::string>(c._M_start);
            size_t space_idx = start_ts.find(" ");
            _M_out << "START DATE: " << start_ts.substr(0, space_idx) << "\n";
            _M_out << "START TIME: " << start_ts.substr(space_idx + 1) << "\n";
            std::string end_ts = static_cast<std::string>(c._M_end);
            space_idx = end_ts.find(" ");
            _M_out << "END DATE: " << end_ts.substr(0, space_idx) << "\n";
            _M_out << "END TIME: " << end_ts.substr(space_idx + 1) << "\n";
            _M_out << "DANCER MATCHING AMOUNT: " << c._M_dancer_amt << "\n";
            _M_out << "GENERAL MATCHIG AMOUNT: " << c._M_general_amt << "\n";
            _M_out << "MAX PER DONATION: " << c._M_max_per_donation << "\n";
            _M_out << "MAX PER DONOR: " << c._M_max_per_donor << "\n";
            _M_out << "MAX PER DANCER: " << c._M_max_per_person << "\n";
            _M_out << "END CRITERION" << "\n";
        }
    }
}