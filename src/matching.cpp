#include "include/matching.h"

namespace GTD
{
    const matching_criterion_t matcher::NO_MATCHING = {ZERO, ZERO, ZERO, ZERO, ZERO, date_tme_t(), date_time_t()};

    matcher::matcher(const std::vector<donation_t>& donation_list, 
        const std::vector<matching_criterion_t>& matching_rounds,
        const std::vector<std::pair<date_time_t, date_time_t>>& matching_round_times)
        : _M_donations(donation_list),
        _M_matching_rounds(matching_rounds),
        _M_curr_general_matching_amt(),
        _M_curr_dancer_matching_amt(),
        _M_alumni_donations(),
        _M_dancer_types(),
        _M_matching_info(),
        _M_donors(),
        _M_alumni(),
        _M_hour_statistics(),
        _M_dancer_statistics()
        {

        }

    matcher::matcher(std::vector<donation_t>&& donation_list, 
        std::vector<matching_criterion_t>&& matching_rounds,
        std::vector<std::pair<date_time_t, date_time_t>>&& matching_round_times)
        : _M_donations(donation_list),
        _M_matching_rounds(matching_rounds),
        _M_curr_general_matching_amt(),
        _M_curr_dancer_matching_amt(),
        _M_alumni_donations(),
        _M_dancer_types(),
        _M_matching_info(),
        _M_donors(),
        _M_alumni(),
        _M_hour_statistics(),
        _M_dancer_statistics()
        {
            
        }

    const std::unordered_map<std::string, dancer_t>& matcher::get_matching_information() const
    {
        return _M_matching_info;
    }

    donation_val_t matcher::dancer_match(donation_val_t donation_amt, donation_val_t donor_matched_amt, donation_val_t dancer_matched_amt)
    {
        auto max_dancer_matching_amt = _M_curr_criterion._M_max_per_person;
        auto max_per_donor = _M_curr_criterion._M_max_per_donor;
        auto max_per_donation = _M_curr_criterion._M_max_per_donation;

        if (dancer_matched_amt >= max_dancer_matching_amt || donor_matched_amt >= max_per_donor) return ZERO;
        if (donation_amt > max_per_donation)
            donation_amt = max_per_donation;
        donation_val_t matched_amt(0,0);
        donation_val_t virtual_matched_amt(0,0);
        //Calculate amount dancer could be matched
        if (dancer_matched_amt + donation_amt < max_dancer_matching_amt) { 
            if (donation_amt + donor_matched_amt < max_per_donor)
                virtual_matched_amt = donation_amt;
            else
                virtual_matched_amt = max_per_donor - donor_matched_amt;
        } else {
            virtual_matched_amt = max_dancer_matching_amt - dancer_matched_amt;
            if (virtual_matched_amt + donor_matched_amt >= max_per_donor)
                virtual_matched_amt = max_per_donor - donor_matched_amt;
        }
        //Calculate amount dancer will be matched based on remaining funds
        if (_M_curr_dancer_matching_amt >= virtual_matched_amt) 
        {
            matched_amt = std::min(virtual_matched_amt, _M_curr_dancer_matching_amt);
            _M_curr_dancer_matching_amt = _M_curr_dancer_matching_amt - matched_amt;
        }
        else if (_M_curr_dancer_matching_amt < virtual_matched_amt && _M_curr_dancer_matching_amt > ZERO)
        {
            donation_val_t dancer_pool_amt = _M_curr_dancer_matching_amt;
            _M_curr_dancer_matching_amt = ZERO;
            virtual_matched_amt = virtual_matched_amt - dancer_pool_amt;
            donation_val_t general_pool_amt = std::min(virtual_matched_amt, _M_curr_general_matching_amt);
            _M_curr_general_matching_amt = _M_curr_general_matching_amt - general_pool_amt;
            matched_amt = general_pool_amt + dancer_pool_amt;
        } else {
            matched_amt = std::min(virtual_matched_amt, _M_curr_general_matching_amt);
            _M_curr_general_matching_amt = _M_curr_general_matching_amt - matched_amt;
        }
        return matched_amt;
    }

    donation_val_t matcher::steering_match(donation_val_t donation_amt, donation_val_t donor_matched_amt, donation_val_t steering_matched_amt)
    {
        auto max_dancer_matching_amt = _M_curr_criterion._M_max_per_person;
        auto max_per_donor = _M_curr_criterion._M_max_per_donor;
        auto max_per_donation = _M_curr_criterion._M_max_per_donation;

        if (steering_matched_amt >= max_dancer_matching_amt || donor_matched_amt >= max_per_donor) return ZERO;
        donation_val_t matched_amt;
        donation_val_t virtual_matched_amt;
        if (donation_amt > max_per_donation)
            donation_amt = max_per_donation;
        //Calculated amount dancer chould be matched 
        if (steering_matched_amt + donation_amt < max_dancer_matching_amt) { 
            if (donation_amt + donor_matched_amt < max_per_donor)
                virtual_matched_amt = donation_amt;
            else
                virtual_matched_amt = max_per_donor - donor_matched_amt;
        } else {
            virtual_matched_amt = max_dancer_matching_amt - steering_matched_amt;
            if (virtual_matched_amt + donor_matched_amt >= max_per_donor)
                virtual_matched_amt = max_per_donor - donor_matched_amt;
        }
        matched_amt = std::min(virtual_matched_amt, _M_curr_general_matching_amt);
        _M_curr_general_matching_amt = _M_curr_general_matching_amt - matched_amt;
        return matched_amt;
    }

    void matcher::reset_matching_pools()
    {
        _M_matching_rounds.pop_back();
        if(!_M_matching_rounds.empty())
        {
            _M_curr_criterion = _M_matching_rounds.back();
            _M_curr_general_matching_amt = _M_curr_criterion._M_general_amt;
            _M_curr_dancer_matching_amt = _M_curr_criterion._M_dancer_amt;
        } 
        else 
        {
            _M_curr_criterion = NO_MATCHING;
            _M_curr_general_matching_amt = ZERO;
            _M_curr_dancer_matching_amt = ZERO;
        }
    }
}

