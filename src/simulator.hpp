#pragma once
#include "card.hpp"
#include <bitset>
#include "util.hpp"
#include "referee.hpp"

class PackOfCards
{
public:
    template<typename T, typename ...Ts>
    void deal(T&& arg, Ts&& ...args)
    {
        assert(!cardsBits_.test(arg));
        cardsBits_.set(arg);
        if constexpr (sizeof...(args) > 0)
        {
            deal(std::forward<Ts>(args)...);
        }
    }

    uint8_t randomDeal()
    {
        uint8_t rdm = randomCard();
        while (cardsBits_.test(rdm))
        {
            rdm = randomCard();
        }
        cardsBits_.set(rdm);
        return rdm;
    }

private:
    std::bitset<52> cardsBits_;
};

template<size_t Gamblers>
class Simulator
{
public:
    
    Simulator(const HandCard& hCard, int times) 
    : handCard_(hCard), flopCard_(),turnCard_(),riverCard_(), 
        simulatorTimes_(times), rates{0,0,0}
    {}

    void addFlopCard(const FlopCard& cards)
    {
        flopCard_ = cards;
    }

    void addTurnCard(const TurnCard& cards)
    {
        turnCard_ = cards;
    }

    void addRiverCard(const RiverCard& cards)
    {
        riverCard_ = cards;
    }

    void run()
    {
        int winCount = 0;
        int equalCount = 0;
        Referee<Gamblers> referee;

        for (int i = 0; i < simulatorTimes_; i++)
        {
            auto hCard = handCard_.getCards();
            PackOfCards packCard;
            packCard.deal(hCard.at(0).getIndex(),hCard.at(1).getIndex());
            std::array<HandCard, Gamblers> otherGamblersHandCards;
            for (size_t i = 0; i < Gamblers; i++)
            {
                auto f = packCard.randomDeal();
                auto s = packCard.randomDeal();
                otherGamblersHandCards[i] = HandCard{f,s};
            }

            FlopCard flopCard = flopCard_.isVaild() ? flopCard_: 
                FlopCard{packCard.randomDeal(), packCard.randomDeal(), packCard.randomDeal()};
            
            TurnCard turnCard = turnCard_.isVaild() ? turnCard_: 
                TurnCard{packCard.randomDeal()};

            RiverCard riverCard = riverCard_.isVaild() ? riverCard_: 
                RiverCard{packCard.randomDeal()};
            

            auto boardCard = flopCard + turnCard + riverCard;

            referee.setHnadCard(handCard_, otherGamblersHandCards);
            referee.setBoardCard(boardCard);
            int result = referee.showdown();
            if(result == 1)
            {
                winCount++;
            }else if (result == 0)
            {
                equalCount++;
            }
        }
        
        double winRate = static_cast<double>(winCount) * 100/ simulatorTimes_;
        double equalRate = static_cast<double>(equalCount) * 100/ simulatorTimes_;
        double lossRate = static_cast<double>(100)  - winRate - equalRate;
        rates = {winRate, equalRate, lossRate};
    }

    const std::tuple<double, double, double>&
    getRates()
    {
        return rates;
    }

private:

    HandCard handCard_;
    FlopCard flopCard_;
    TurnCard turnCard_;
    RiverCard riverCard_;
    int simulatorTimes_;
    std::tuple<double, double, double> rates;
};