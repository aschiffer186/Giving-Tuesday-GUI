#include "Command_Line_UI/ui.hpp"

int main(int argc, char** argv)
{
    #ifndef FUNDRAISING_USE_UI
        Fundraising::Command_Line::command_line_run(argc, argv);
    #else
        Fundraising::UI::run();
    #endif
    return 0;
}