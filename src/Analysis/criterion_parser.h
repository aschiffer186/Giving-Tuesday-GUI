#ifndef CRITERION_PARSER_H
#define CRITERION_PARSER_H 1

#include "matching_base.h"
#include <tuple>
#include <istream>
#include <vector>

namespace Fundraising::Analysis
{
    class parser
    {
        public:
            parser(std::istream& in);
            std::vector<matching_criterion_t> parse_criteria();
        private:
            void reset();
            static bool find_ignore_case(const std::string& target, const std::string& goal);
            static std::tuple<int,short,short> parse_date(const std::string& str);
            static std::tuple<short,short,short> parse_time(const std::string& str);
            bool check_file_validity() const;
        private:
            //Input stream parser is connected to
            std::istream& _M_in;
            //State flags 
            bool seenBegin;
            bool seenEnd;
            bool seenMaxPerDancer;
            bool seenMaxPerDonor;
            bool seenMaxPerDonation;
            bool seenGenMatching;
            bool seenDancerMatching;
    };

    class writer
    {
        public: 
            writer(std::ostream& os);
            void write_criteria(const std::vector<matching_criterion_t>& vec);
        private:
            std::ostream& _M_out;
    };
}

#endif