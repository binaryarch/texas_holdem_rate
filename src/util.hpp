#pragma once
#include <array>
#include "card.hpp"
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>

template<size_t S>
void printCards(const CardCenter<S>& cc)
{
    for (const auto& card: cc.getCards())
    {
        std::cout << card << " ";
    }
    std::cout << std::endl;
}

uint8_t randomCard()
{
    static thread_local size_t i = 0;
    static thread_local long rnd = 0;
    constexpr size_t maxI = sizeof(long) * 8 ;
    if(i == 0)
    {
        auto now = std::chrono::steady_clock::now();
        srandom(now.time_since_epoch().count());
        rnd = random();
    }

    auto res = static_cast<uint8_t>(((rnd >> (i++)) & 0x3F) % 52);
    i%=maxI;

    return res;
}
