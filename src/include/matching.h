#ifndef MATCHING_HH
#define MATCHING_HH 1

#include <memory> //For unique_ptr
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "basic_types.h"

namespace GTD 
{

    struct matching_criterion_t
    {
        double _M_leadership_percentage;
        double _M_dancer_percentage;
        donation_val_t _M_amt;
        donation_val_t _M_max_per_donor;
        donation_val_t _M_max_per_person;
        donation_val_t _M_max_per_donation;
    };

    //Total Donations, Mean Donation, Median Donation,  % of Total Fundraising, Num Participants, % of Total Participants
    #define DANCER_STATISTICS_ROW output_row_t<donation_val_t, donation_val_t, donation_val_t, double, size_t, double>
   //Hourly fundraising, mean donation size, median donation size, num donors, num unique donors, number of alumni donors, 
   //number of unique alumni donors
    #define HOURLY_STATISTICS_ROW output_row_t<donation_val_t, donation_val_t, donation_val_t, int, int, int, init>

    class matcher
    {
        public:
            matcher(const std::vector<donation_t>& donation_list, 
            const std::vector<matching_criterion_t>& matching_rounds,
            const std::vector<std::pair<date_time_t, date_time_t>>& matching_round_times);

            matcher(std::vector<donation_t>&& donation_list, 
            std::vector<matching_criterion_t>&& matching_rounds,
            std::vector<std::pair<date_time_t, date_time_t>>&& matching_round_times);
        private: //Helper functions 
        //Calculates the amount that the dancer should be matched and subtracts from 
        //the appropriate matching pool. 
        //@param donation_amt the size of the donation_amt 
        //@param donor_matched_amt the amount the donor has already donated to the dancer 
        //@param dancer_matched_amt the amount the dancer has already been matched
        //@return donation_amt the amount the dancer should be matched
        donation_val_t dancer_match(donation_val_t donation_amt, donation_val_t donor_matched_amt, donation_val_t dancer_matched_amt);

        //Calculates the amount that a steering member should be matched and subtracts from 
        //the appropriate matching pool. 
        //
        //@param donation_amt the size of the donation_amt 
        //@param donor_matched_amt the amount the donor has already donated to the dancer 
        //@param dancer_matched_amt the amount the dancer has already been matched
        //@return donation_amt the amount the dancer should be matched
        donation_val_t steering_match(donation_val_t donation_amt, donation_val_t donor_matched_amt, donation_val_t steering_matched_amt);
        //Returns the amount a donor has donated to the specified dancer 
        //
        //@param dancer the specified dancer 
        //@param donor the specified donor
        //@return the amount a donor has specified to a particular dancer
        donation_val_t get_donation_info(const dancer_t& dancer, const donor_t& donor);
        //Updates the amount the specified donor has donated to the dancer
        //@param dancer the specified dancer 
        //@param donor the specified donor
        //@param amt the amount in the donation from the donor to the dancer
        void update_donation_info(dancer_t& dancer, const donor_t& donor, donation_val_t amt);
        //Updates the dancer statistics information with the dancers new fundraising total
        //@param dancer the dancer whose total will be used to update the fundraising statistics
        void update_dancer_statistics(const dancer_t& dancer, const donation_val_t& d);
        
        void update_dancer_statistics_row(dancer_t dancer, const donation_val_t& d, DANCER_STATISTICS_ROW& row);
        DANCER_STATISTICS_ROW make_dancer_statistics_row(const dancer_t& dancer, const donation_val_t& donation);
        private:
            static const matching_criterion_t NO_MATCHING;
        private:
            //List of donations
            std::vector<donation_t> _M_donations;
            //Matching criteria 
            std::vector<matching_criterion_t> _M_matching_rounds;
            std::vector<std::pair<date_time_t, date_time_t>> _M_matching_round_times;
            donation_val_t _M_curr_general_matching_amt;
            donation_val_t _M_curr_dancer_matching_amt;
            //Statistic keeping information 
            std::vector<donor_t> _M_donors;
            std::vector<donor_t> _M_alumni;
            std::vector<std::vector<std::unordered_set<std::string>>> _M_alumni_donations;
            std::unordered_map<std::string, std::unordered_set<dancer_t>> _M_dancer_types;
            std::unordered_map<date_time_t, std::vector<donation_t>> _M_donations_per_hour;

    };

    typedef std::unique_ptr<matcher> matcher_ptr; //will want lazy initialization later
}

#endif