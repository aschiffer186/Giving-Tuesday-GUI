#ifndef BASIC_TYPES_H
#define BASIC_TYPES_H

#include <tuple>

//Date and time constructos
struct date_time
{
    std::tuple<short,short,short> _M_date;
    std::tuple<short,short,short> _M_time;
};

bool operator<(const date_time& lhs, const date_time& rhs);



#endif