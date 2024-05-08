#include <iostream>
#include "card.hpp"
#include "rules.hpp"
#include "util.hpp"
#include <array>
#include "simulator.hpp"

int main(int argc, char const *argv[])
{

    HandCard hCard{1,15};

    Simulator<6> simulator(hCard, 100000);
    simulator.run();
    auto rates = simulator.getRates();

    std::cout << "Win:\t" << std::get<0>(rates) << std::endl;
    std::cout << "Equal:\t" << std::get<1>(rates) << std::endl;
    std::cout << "Loss:\t" << std::get<2>(rates) << std::endl;

    return 0;
}
