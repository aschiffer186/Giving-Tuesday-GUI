#include "basic_types.h"
#include <string>

std::ostream& operator<<(std::ostream& os, const donation_val_t& d)
{
    std::string cents_string;
    if (d.second < 10) cents_string = "0" + std::to_string(d.second);
    else cents_string = std::to_string(d.second);
    os << "$" << std::to_string(d.first) << "." << cents_string;
    return os;
}

donation_val_t operator+(const donation_val_t& lhs, const donation_val_t& rhs)
{
    int dollars = lhs.first + rhs.first;
    int cents = lhs.second + rhs.second;
    if (cents >= 100) 
    {
        cents = cents - 100;
        dollars += 1;
    }
    return std::make_pair(dollars, cents);
}

donation_val_t operator-(const donation_val_t& lhs, const donation_val_t& rhs)
{
    int dollars = lhs.first - rhs.first;
    int cents = lhs.second - rhs.second;
    if (cents < 0)
    {
        cents += 100;
        dollars -= 1;
    }
    return std::make_pair(dollars, cents);
}

bool operator<(const donation_val_t& lhs, const donation_val_t& rhs)
{
    if (lhs.first < rhs.first) 
        return true;
    if (lhs.first == rhs.first)
        return lhs.second < rhs.second;
    return false;
}

bool operator>(const donation_val_t& lhs, const donation_val_t& rhs)
{
    if (lhs.first > rhs.first) 
        return true;
    if (lhs.first == rhs.first)
        return lhs.second > rhs.second;
    return false;
}

bool operator<=(const donation_val_t& lhs, const donation_val_t& rhs)
{
    return !(lhs > rhs);
}

bool operator>=(const donation_val_t& lhs, const donation_val_t& rhs)
{
    return !(lhs < rhs);
}

bool operator==(const donation_val_t& lhs, const donation_val_t& rhs)
{
    return (lhs <= rhs) && (lhs >= rhs);
}

donation_val_t make_donation(const std::string& value)
{
    size_t decimal_index = value.find('.');
    int dollar_val = std::stoi(value.substr(0, decimal_index));
    int cents;
    if (decimal_index == std::string::npos) cents = 0;
    else {
        cents = std::stoi(value.substr(decimal_index + 1));
        if (decimal_index == value.size() - 2) cents *= 10;
    }
    return std::make_pair(dollar_val, cents);
}

donation_val_t make_donation(int dollar, int cents)
{
    return std::make_pair(dollar, cents);
}

std::ostream& operator<<(std::ostream& os, const donation_t& d)
{
    os << "{";
    //Make timestamp
    date_time_t dt = d._M_timestamp;
    short hour = std::get<0>(dt._M_time);
    short min = std::get<1>(dt._M_time);
    short sec = std::get<2>(dt._M_time);

    int year = std::get<0>(dt._M_date);
    short month = std::get<1>(dt._M_date);
    short day = std::get<2>(dt._M_date);

    std::string year_string = std::to_string(year);
    std::string month_string = (month < 10) ? "0" + std::to_string(month) : std::to_string(month);
    std::string day_string = (day < 10) ? "0" + std::to_string(day) : std::to_string(day);

    std::string hour_string = (hour == 0) ? "00" : std::to_string(hour);
    std::string min_string = (min == 0) ? "00" : std::to_string(min);
    std::string sec_string = (sec == 0) ? "00" : std::to_string(sec);
    //Output information
    os << "Timestamp: " << year_string << "/" << month_string << "/" << day_string <<
        hour_string << ":" << min_string << ":" << sec_string << ", ";
    os << "Amount: " << (d._M_amt) << ", ";
    os << "Donor Name: " << d._M_donor_first_name << " " << d._M_donor_last_name << ", ";
    os << "Donor Email: "<< d._M_donor_email << ", ";
    os << "Donor Relation: " << d._M_donor_relation << ", ";
    os << "Dancer Name: " << d._M_dancer_name << ", ";
    os << "Dancer House: " << d._M_dancer_house << ", ";
    os << "Dancer Team: " << d._M_dancer_team << ", ";
    os << "Dancer Type: " << d._M_dancer_role << ", ";
    os << "Dancer ID: " << d._M_dancer_id;
    os << "}";
    return os;
}

donation_t::donation_t(const date_time_t& timestamp,
            const donation_val_t& amt, 
            const std::string& donor_first_name, 
            const std::string& donor_last_name,
            const std::string& donor_email,
            const std::string& donor_phone,
            const std::string& donor_card,
            const std::string& donor_relation,
            const std::string& dancer_name,
            const std::string& dancer_email,
            const std::string& dancer_house,
            const std::string& dancer_team,
            const std::string& dancer_type,
            const std::string& dancer_id) :
            _M_timestamp(timestamp), 
            _M_amt(amt),
            _M_donor_first_name(donor_first_name),
            _M_donor_last_name(donor_last_name),
            _M_donor_email(donor_email),
            _M_donor_phone(donor_phone),
            _M_donor_card(donor_card),
            _M_donor_relation(donor_relation),
            _M_dancer_name(dancer_name),
            _M_dancer_email(dancer_email),
            _M_dancer_house(dancer_house),
            _M_dancer_team(dancer_team),
            _M_dancer_role(dancer_type),
            _M_dancer_id(dancer_id)
            {

            }

donor_t::donor_t(const std::string& first_name,
        const std::string& last_name,
        const std::string& email,
        const std::string& phone, 
        const std::string& card,
        const std::string& relation) :
        _M_donor_first_name(first_name),
        _M_donor_last_name(last_name),
        _M_donor_email(email),
        _M_donor_phone(phone),
        _M_donor_card(card),
        _M_donor_relation(relation),
        _M_donation_amt(ZERO),
        _M_matched_amt(ZERO)
        {

        }

bool operator==(const donor_t& lhs, const donor_t& rhs)
{
    if (lhs._M_donor_phone == rhs._M_donor_phone) 
        return true;
    if (lhs._M_donor_email == rhs._M_donor_email) 
        return true;
    if (lhs._M_donor_card == rhs._M_donor_card) 
        return lhs._M_donor_last_name == rhs._M_donor_last_name;
    return false;
}

bool operator!=(const donor_t& lhs, const donor_t& rhs)
{
    return !(lhs==rhs);
}

dancer_t::dancer_t(const std::string& dancer_id,
        const std::string& name,
        const std::string& email,
        const std::string& role,
        const std::string& house, 
        const std::string& team
        ) :
        _M_dancer_id(dancer_id),
        _M_dancer_name(name),
        _M_dancer_email(email),
        _M_dancer_role(role),
        _M_dancer_house(house),
        _M_dancer_team(team),
        _M_amt_raised(ZERO),
        _M_amt_matched(ZERO)
        {

        }