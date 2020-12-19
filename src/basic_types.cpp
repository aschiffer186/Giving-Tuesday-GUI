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