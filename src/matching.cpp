#include "include/matching.h"
#include <numeric>
#include <algorithm>

namespace GTD
{
    const matching_criterion_t matcher::NO_MATCHING = {ZERO, ZERO, ZERO, ZERO, ZERO, date_time_t(), date_time_t()};

    bool matcher::is_no_matching(const matching_criterion_t& mc)
    {
        return mc._M_dancer_amt == ZERO && mc._M_general_amt == ZERO && mc._M_max_per_donation == ZERO
            && mc._M_max_per_donor == ZERO && mc._M_max_per_person == ZERO;
    }

    matcher::matcher(const std::vector<donation_t>& donation_list, 
        const std::vector<matching_criterion_t>& matching_rounds)
        : _M_donations(donation_list),
        _M_matching_rounds(matching_rounds),
        _M_curr_general_matching_amt(),
        _M_curr_dancer_matching_amt(),
        _M_alumni_donations(),
        _M_dancers_by_type(),
        _M_matching_info(),
        _M_donors(),
        _M_alumni(),
        _M_hour_statistics(),
        _M_dancer_statistics()
        {
            initialize_matching_pools();
        }

    matcher::matcher(std::vector<donation_t>&& donation_list, 
        std::vector<matching_criterion_t>&& matching_rounds)
        : _M_donations(donation_list),
        _M_matching_rounds(matching_rounds),
        _M_curr_general_matching_amt(),
        _M_curr_dancer_matching_amt(),
        _M_alumni_donations(),
        _M_dancers_by_type(),
        _M_matching_info(),
        _M_donors(),
        _M_alumni(),
        _M_hour_statistics(),
        _M_dancer_statistics()
        {
            initialize_matching_pools();
        }
    
    void matcher::initialize_matching_pools()
    {
        if (_M_matching_rounds.empty())
        {
            zero_matching_pools();
        } else {
            auto timestamp = _M_donations.front()._M_timestamp;
            if (timestamp < _M_matching_rounds.back()._M_start)
            {
               zero_matching_pools();
            } else 
            {
                _M_curr_criterion = _M_matching_rounds.back();
                _M_curr_general_matching_amt = _M_curr_criterion._M_general_amt;
                _M_curr_dancer_matching_amt = _M_curr_criterion._M_dancer_amt;
            }
        }
    }

    const std::unordered_map<std::string, dancer_t>& matcher::get_matching_information() const
    {
        return _M_matching_info;
    }

    const std::unordered_map<std::string, dancer_statistics_row>& matcher::get_dancer_statistics() const
    {
        return _M_dancer_statistics;
    }

    const std::unordered_map<date_time_t, HOURLY_STATISTICS_ROW>& matcher::get_hourly_statistics() const
    {
        return _M_hour_statistics;
    }

    const std::vector<donor_t>& matcher::get_donor_information() const
    {
        return _M_donors;
    }

    std::vector<std::pair<date_time_t, donation_val_t>> matcher::get_general_matching_money_left() const
    {
        return _M_unused_general;
    }

    std::vector<std::pair<date_time_t, donation_val_t>> matcher::get_dancer_matching_money_left() const
    {
        return _M_unused_dancer;
    }

    void matcher::perform_matching_calculations()
    {
        //Iterator through all donations
        std::pair<decltype(_M_donations.begin()), decltype(_M_donations.end())> curr_hour_donations;
        for (size_t i = 0; i < _M_donations.size(); ++i)
        {
            donation_t donation = _M_donations[i];
            if (i == 0)
                curr_hour_donations.first = _M_donations.begin();
            else 
            {
                donation_t prev = _M_donations[i-1];
                short prev_hour = prev._M_timestamp.get_hour();
                short curr_hour = donation._M_timestamp.get_hour();
                if(curr_hour > prev_hour)
                {
                    curr_hour_donations.second = _M_donations.begin() + i;
                    _M_donations_by_hours[curr_hour_donations.first->_M_timestamp] = curr_hour_donations;
                    curr_hour_donations.first = curr_hour_donations.second;
                }
                if (i == _M_donations.size() - 1)
                {
                    curr_hour_donations.second = _M_donations.end();
                    _M_donations_by_hours[curr_hour_donations.first->_M_timestamp] = curr_hour_donations;
                }
            }
            //Check to see if need to reset matching pools
            //Not thrilled with this solution but whatever 
            if(!_M_matching_rounds.empty())
            {
              if(!is_no_matching(_M_curr_criterion)) 
              {
                if(donation._M_timestamp > _M_curr_criterion._M_end)
                    reset_matching_pools(donation._M_timestamp);
              } 
              else 
              {
                if(donation._M_timestamp >= _M_matching_rounds.back()._M_start)
                    reset_matching_pools(donation._M_timestamp);
              }
            }
            //Get dancer and donor info
            dancer_t dancer;
            auto d_it = _M_matching_info.find(donation._M_dancer_id);
            if (d_it == _M_matching_info.end()){
                dancer = {
                    donation._M_dancer_id,
                    donation._M_dancer_name,
                    donation._M_dancer_email,
                    donation._M_dancer_role,
                    donation._M_dancer_house,
                    donation._M_dancer_team
                };
            } else 
            {
                dancer = d_it->second;
            }
            donor_t donor(
                donation._M_donor_first_name,
                donation._M_donor_last_name,
                donation._M_donor_email,
                donation._M_donor_phone,
                donation._M_donor_card,
                donation._M_donor_relation
            );
            //Update amount raised 
            _M_total_raised = _M_total_raised + donation._M_amt;
            //Calculate matching 
            donation_val_t donor_matched_amt = get_donation_info(dancer, donor);
            std::string role = dancer._M_dancer_role;
            donation_val_t matched_amt;
            if (role == "Dancer") matched_amt =dancer_match(donation._M_amt, donor_matched_amt, dancer._M_amt_matched);
            else if (role != "DMUM") matched_amt = steering_match(donation._M_amt, donor_matched_amt, dancer._M_amt_matched);
            //Update dancer matching 
            dancer._M_amt_matched = dancer._M_amt_matched + matched_amt;
            dancer._M_amt_raised = dancer._M_amt_raised + donation._M_amt;
            update_donation_info(dancer, donor, matched_amt);
            //Update donor matching 
            donor._M_matched_amt = donor._M_matched_amt + matched_amt;
            donor._M_donation_amt = donor._M_donation_amt + donation._M_amt;
            //Update statistics 
            //Dancer mathing information for Finance 
            _M_matching_info[dancer._M_dancer_id] = dancer;
            //update dancer statistics 
            update_dancer_statistics(dancer, donation._M_amt);
            //Update donor statistics
            auto donor_it = std::find(_M_donors.begin(), _M_donors.end(), donor);
            if (donor_it == _M_donors.end())
            {
                _M_donors.push_back(donor);
            } 
            else 
            {
                donor_t& d = *donor_it;
                d._M_donation_amt = d._M_donation_amt + donation._M_amt;
                d._M_matched_amt = d._M_matched_amt + matched_amt;
            }
            //Update alumni info
            if(donor._M_donor_relation.find("DMUM Alumni") != std::string::npos)
            {
                auto alumnus_it = std::find(_M_alumni.begin(), _M_alumni.end(), donor);
                if (alumnus_it == _M_alumni.end())
                {
                    std::unordered_map<std::string, std::unordered_set<std::string>> donation_types;
                    if (dancer._M_dancer_role == "DMUM")
                        donation_types["DMUM"].insert(dancer._M_dancer_id);
                    else if (dancer._M_dancer_role == "Dancer")
                        donation_types["Dancer"].insert(dancer._M_dancer_id);
                    else
                        donation_types["Leadership"].insert(dancer._M_dancer_id);
                    _M_alumni.push_back(donor);
                    _M_alumni_donations.push_back(donation_types);
                } 
                else 
                {
                    donor_t& d = *alumnus_it;
                    d._M_donation_amt = d._M_donation_amt + donation._M_amt;
                    d._M_matched_amt = d._M_matched_amt + matched_amt;
                    size_t index = static_cast<size_t>(alumnus_it - _M_alumni.begin());
                    auto& donation_types = _M_alumni_donations[index];
                    if (dancer._M_dancer_role == "DMUM")
                        donation_types["DMUM"].insert(dancer._M_dancer_id);
                    else if (dancer._M_dancer_role == "Dancer")
                       donation_types["Dancer"].insert(dancer._M_dancer_id);
                    else
                       donation_types["Leadership"].insert(dancer._M_dancer_id);
                }
            }
        }
        //Build statistics
        generate_dancer_statistics();
        generate_hour_statistics();
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

    void matcher::reset_matching_pools(const date_time_t& dt)
    {
        //Add to unused amounts
        if (!is_no_matching(_M_curr_criterion)) 
        {
            _M_unused_general.emplace_back(_M_curr_criterion._M_start, _M_curr_general_matching_amt);
            _M_unused_dancer.emplace_back(_M_curr_criterion._M_start, _M_curr_dancer_matching_amt);
        }
        _M_matching_rounds.pop_back();
        if(!_M_matching_rounds.empty())
        {
            //Check if matching criterion immediately following current one 
            if (dt >= _M_matching_rounds.back()._M_start)
            {
                _M_curr_criterion = _M_matching_rounds.back();
                _M_curr_general_matching_amt = _M_unused_general.back().second + _M_curr_criterion._M_general_amt;
                _M_curr_dancer_matching_amt = _M_unused_dancer.back().second + _M_curr_criterion._M_dancer_amt;
            } 
            else //We're in between matching criteria 
            {
                zero_matching_pools();
            }
        } 
        //Done with matching for duration of program
        else 
        {
           zero_matching_pools();
        }
    }

    void matcher::zero_matching_pools()
    {
        _M_curr_criterion = NO_MATCHING;
        _M_curr_general_matching_amt = ZERO;
        _M_curr_dancer_matching_amt = ZERO;
    }

    donation_val_t matcher::get_donation_info(const dancer_t& dancer, const donor_t& donor)
    {
         auto it = std::find_if(dancer._M_donors.begin(), dancer._M_donors.end(),
            [&](const std::pair<donor_t, donation_val_t>& lhs) {return lhs.first == donor;});
        if (it == dancer._M_donors.end()) return ZERO;
        return it->second;
    }

    void matcher::update_donation_info(dancer_t& dancer, const donor_t& donor, donation_val_t amt)
    {
        auto it = std::find_if(dancer._M_donors.begin(), dancer._M_donors.end(), 
            [&](const std::pair<donor_t, donation_val_t>& lhs) {return lhs.first == donor;});
        if (it == dancer._M_donors.end()) dancer._M_donors.emplace_back(donor, amt);
        else 
        {
            donation_val_t donor_amt = it->second;
            donor_amt = donor_amt + amt;
            it->second = donor_amt;
        }
    }

    void matcher::update_dancer_statistics(const dancer_t& dancer, const donation_val_t& d)
    {
        std::string role = dancer._M_dancer_role;
        if(role == "DMUM") return;
        //Update based on role
        update_statistics_table(dancer, d, role);
        //Update based on house 
        std::string house = dancer._M_dancer_house;
        update_statistics_table(dancer, d, house);
        //Update leadership role is needed 
        if(role != "Dancer")
        {
           update_statistics_table(dancer, d, "Leadership");
        }
    }

    void matcher::update_statistics_table(const dancer_t& dancer, const donation_val_t&d, const std::string& role)
    {
        auto it = _M_dancers_by_type.find(role);
        if (it == _M_dancers_by_type.end())
        {
            _M_dancers_by_type[role] = {dancer};
        }
        else 
        {
            auto dancers_role_it = _M_dancers_by_type.find(role);
            auto dancer_it = dancers_role_it->second.find(dancer);
            if (dancer_it == dancers_role_it->second.end())
            {
                dancers_role_it->second.insert(dancer);
            }
            else 
            {
                dancers_role_it->second.erase(dancer_it);
                dancers_role_it->second.insert(dancer);
            }
        }
    }

    void matcher::generate_dancer_statistics()
    {
        double total_participants = _M_matching_info.size();
        double total_raised = _M_total_raised.first + _M_total_raised.second*0.01;
        auto it1 = _M_dancers_by_type.begin();
        while (it1 != _M_dancers_by_type.end())
        {
            std::string role = it1->first;
            auto dancer_set = it1->second;
            donation_val_t total_donations = std::accumulate(dancer_set.begin(), dancer_set.end(), std::make_pair(0,0), 
                [](const donation_val_t& lhs, const dancer_t& rhs) {return lhs + rhs._M_amt_raised;});
            donation_val_t avg_donation = total_donations/dancer_set.size();
            std::vector<donation_val_t> donation_list;
            std::sort(donation_list.begin(), donation_list.end());
            donation_val_t median_donation;
            if (donation_list.size() == 1)
                median_donation = donation_list.front();
            else if (donation_list.size() % 2 == 1) 
                median_donation = donation_list[donation_list.size() / 2];
            else 
                median_donation = (donation_list[donation_list.size()/2 - 1] + donation_list[donation_list.size()/2])/2;
            size_t num_participants = dancer_set.size();
            double type_fundraising = total_donations.first + total_donations.second*0.01;
            double percent_of_total = type_fundraising/total_raised;
            double percent_of_participants = num_participants/total_participants;
            _M_dancer_statistics[role] = std::make_tuple(total_donations, avg_donation, median_donation, 
                percent_of_total, num_participants, percent_of_participants);
        }
    }

    void matcher::generate_hour_statistics()
    {
        for(const auto& hour: _M_donations_by_hours)
        {
            auto donations_start = hour.second.first;
            auto donation_end = hour.second.second;
            donation_val_t total_raised;
            size_t num_donations = 0;
            size_t num_alumni_donations = 0;
            std::unordered_set<std::string> unique_donors;
            std::unordered_set<std::string> unique_alumni_donors;
            std::vector<donation_val_t> donation_list;
            for(; donations_start != donation_end; ++donations_start)
            {
                total_raised = total_raised + donations_start->_M_amt;
                donation_list.push_back(donations_start->_M_amt);
                ++num_donations;
                unique_donors.insert(donations_start->_M_donor_phone);
                if (donations_start->_M_donor_relation.find("DMUM Alumni") != std::string::npos) 
                {
                    ++num_alumni_donations;
                    unique_alumni_donors.insert(donations_start->_M_donor_phone);
                }
            }
            std::sort(donation_list.begin(), donation_list.end());
            donation_val_t median_donation;
            if (donation_list.size() == 1)
                median_donation = donation_list.front();
            else if (donation_list.size() % 2 == 1) 
                median_donation = donation_list[donation_list.size() / 2];
            else 
                median_donation = (donation_list[donation_list.size()/2 - 1] + donation_list[donation_list.size()/2])/2;
            donation_val_t avg_donation = total_raised/num_donations;
            _M_hour_statistics[hour.first] = std::make_tuple(total_raised, avg_donation, median_donation, num_donations, 
                unique_donors.size(), num_alumni_donations, unique_alumni_donors.size());
        }
    }
}

