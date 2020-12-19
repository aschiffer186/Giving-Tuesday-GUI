#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <tuple>
#include <ostream>
#include <type_traits>
#include <concepts>
#include <cmath>
#include <string>

//Type to represent date and time
struct date_time_t
{
    //Date in year-month-day format
    std::tuple<short,short,short> _M_date;
    //Time in hour-min-second dformat
    std::tuple<short,short,short> _M_time;
    //Implicitly covert date_time sto std::string 
    //@return date_time_t in format yyyy/mm/dd hh:mm:ss
    operator std::string() const;
};

std::ostream& operator<<(std::ostream& os, const date_time_t& dt);
bool operator<(const date_time_t& lhs, const date_time_t& rhs);

//Types to represent a donation's amount
using donation_val_t = std::pair<int, int>;

donation_val_t operator+(const donation_val_t& lhs, const donation_val_t& rhs);
donation_val_t operator-(const donation_val_t& lhs, const donation_val_t& rhs);

template<typename _DTp> requires std::integral<_DTp>
inline donation_val_t operator/(const donation_val_t& lhs, _DTp& rhs)
{
    double dollars = lhs.first;
    double cents = lhs.second;
    //Exploit the fact that abc.d/rhs = abc/rhs + de-2/rhs
    double dollars_result = dollars/rhs;
    int new_dollars = static_cast<int>(dollars_result);
    int cents_result = static_cast<int>(std::round(cents/rhs));
    int new_cents = cents_result + static_cast<int>(std::round((dollars_result - new_dollars)*100));
    return std::make_pair(new_dollars, new_cents);
}

//Comparison operator overloads
bool operator<(const donation_val_t& lhs, const donation_val_t& rhs);
bool operator<=(const donation_val_t& lhs, const donation_val_t& rhs);
bool operator>(const donation_val_t& lhs, const donation_val_t& rhs);
bool operator>=(const donation_val_t& lhs, const donation_val_t& rhs);
bool operator==(const donation_val_t& lhs, const donation_val_t& rhs);

//A struct to represent a single donation 
//Includes information about
//  1) the date and time of the donation 
//  2) the value of the donation
//  3) the donor's name 
//  4) the donor's contact information 
//  5) the donor's relationship to DMUM 
//  6) the person they donated to 
struct donation_t
{

    //Creates a new donation_t
    //@param timestamp the date and time of the donation 
    //@param amt the donation amount 
    //@param donor_first_name the donor's first name
    //@param donor_last_name the donor's last name 
    //@param donor_email the donor's email 
    //@param donor_phone the donor's phone 
    //@param donor_card the last 4 digits of the donor's card
    //@param donor_relation the donor's relationship to DMUM
    //@param dancer_name the recipient's name 
    //@param dancer_email the recipient's email
    //@param dancer_house the receipient's house 
    //@param 
   donation_t(const date_time_t& timestamp,
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
            const std::string& dancer_id);

    //The donation timestamp
    date_time_t _M_timestamp;
    //The amount in the donation
    donation_val_t _M_amt;
    //The donor's name
    std::string _M_donor_first_name;
    //The donors last name 
    std::string _M_donor_last_name;
    //The donor's email
    std::string _M_donor_email;
    //The donor's phone
    std::string _M_donor_phone;
    //The last 4 digits of the donor's card
    std::string _M_donor_card;
    //The donor's relation 
    std::string _M_donor_relation;
    //The dancer's name
    std::string _M_dancer_name;
    //The dancer's email 
    std::string _M_dancer_email;
    //The dancer's house
    std::string _M_dancer_house;
    //The dancer's team
    std::string _M_dancer_team;
    //The dancer's type
    std::string _M_dancer_type;
    //The associated peer id 
    std::string _M_dancer_id;
};

#endif