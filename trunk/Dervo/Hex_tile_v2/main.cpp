#include <ctime>
#include <cstdlib>

#include "icarus/state_manager.hpp"

int main()
{
    //unsigned seed = 0xabcdef;
    unsigned seed = 42;

    srand(seed);
    icarus::state_manager::get()->initialize("settings.cfg");
    icarus::state_manager::get()->start();

    return 0;
}