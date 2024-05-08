#pragma once
#include "card.hpp"
#include "rules.hpp"

template<size_t M>
class Referee
{
public:

    void setHnadCard(HandCard hCard, const std::array<HandCard, M>& otherHnadCard)
    {
        handCard_ = hCard;
        otherHnadCard_ = otherHnadCard;
    }

    void setBoardCard(CardCenter<5> boardCard)
    {
        boardCard_ = boardCard;
    }

    //-1 loss 1 win 0 same
    int showdown()
    {
        int result = 0;
        assert(handCard_.isVaild());
        assert(otherHnadCard_.size() > 0);
        assert(boardCard_.isVaild());

        auto myCard = handCard_ + boardCard_;

        Rules rule(myCard);

        auto[model, resultCard] = rule.getResult();

        for (const auto& hCard: otherHnadCard_)
        {
            auto otherCard = hCard + boardCard_;
            rule.setCardCenter(otherCard);
            auto[mdl, resCard] = rule.getResult();
            if(model > mdl)
            {
                result = -1;
                break;
            }else if(model == mdl)
            {
                result = resultCard.compare(resCard);
            }else
            {
                result = 1;
            }
        }
        return result;
    }
    

private:
    HandCard handCard_;
    std::array<HandCard, M> otherHnadCard_;
    CardCenter<5> boardCard_;
};