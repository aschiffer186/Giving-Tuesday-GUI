#ifndef MATCHING_BASE_H
#define MATCHING_BASE_H 1

namespace Fundraising::Analysis
{
     struct matching_criterion_t
    {
        donation_val_t _M_general_amt;
        donation_val_t _M_dancer_amt;
        donation_val_t _M_max_per_donor;
        donation_val_t _M_max_per_person;
        donation_val_t _M_max_per_donation;
        date_time_t _M_start;
        date_time_t _M_end;
    };
}

#endif