#pragma once

#include <algorithm>
#include "card.hpp"
#include <vector>

enum CardModel:uint8_t
{
    unKnow,
    royalFlush,
    straightFlush,
    fourOfAkind,
    fullHouse,
    flush,
    straight,
    threeOfAkind,
    twoPair,
    onePair,
    hignCard,
};

static constexpr std::array<std::string_view, 11> cardModelStr = 
{
    "unKnow",
    "royalFlush",
    "straightFlush",
    "fourOfAkind",
    "fullHouse",
    "flush",
    "straight",
    "threeOfAkind",
    "twoPair",
    "onePair",
    "hignCard"
};



class Rules
{
public:
    Rules() = default;
    Rules(const CardCenter<7> &cardCenter) : cardCenter_(cardCenter) {}

    void setCardCenter(const CardCenter<7> &cardCenter)
    {
        cardCenter_ = cardCenter;
    }

    std::tuple<uint8_t, ResultCard> getResult()
    {
        std::array<Card,5> result1;
        uint8_t model1 = processSuits(result1);
        if(model1 == royalFlush || model1 == straightFlush)
        {
            return {model1, result1};
        }

        std::array<Card,5> result2;
        uint8_t model2 = processSameValues(result2);
        if(model2 == fourOfAkind || model2 == fullHouse)
        {
            return {model2, result2};
        }

        if(model1 == flush)
        {
            return {model1, result1};
        }

        std::array<Card,5> result3;
        uint8_t model3 = processStraight(result3);

        if(model3 == straight)
        {
            return {model3, result3};
        }

        return {model2, result2};
    }

private:
    enum SortCondition : uint8_t
    {
        cardIndex,
        cardValue,
        cardValueAscAs1,
    };

    void sortCards(std::vector<Card>& cards, uint8_t sortCondition)
    {
        std::sort(cards.begin(), cards.end(),
                  [sortCondition](const Card &c1, const Card &c2)
                  {
                      if (sortCondition == cardIndex)
                      {
                          return c1.getIndex() > c2.getIndex();
                      }
                      else if (sortCondition == cardValueAscAs1)
                      {
                          return c1.getValue(true) > c2.getValue(true);
                      }
                      else
                      {
                          return c1 > c2;
                      }
                  });
    }

    std::vector<Card> getLargestSuit()
    {
        auto sCards = cardCenter_.getCards();
        std::array<std::vector<Card>, 4> cards;
        for (const auto &card : sCards)
        {
            cards[card.getSuit()].push_back(card);
        }

        const auto &ret1 = cards[0].size() > cards[1].size() ? cards[0] : cards[1];
        const auto &ret2 = cards[2].size() > cards[3].size() ? cards[2] : cards[3];
        const auto &ret = ret1.size() > ret2.size() ? ret1 : ret2;
        return ret;
    }

    bool isRoyalFlush(std::vector<Card>& cards, std::array<Card, 5> &result)
    {
        sortCards(cards, cardValue);
        size_t i = 0;
        for (; i < cards.size(); i++)
        {
            if((cards[i].getValue() == (13 - i)) && i < 5)
            {
                result[i] = cards[i];
            }else
            {
                break;
            }
        }
        return i == 5;
    }

    bool isStraightFlush(std::vector<Card>& cards, 
        std::array<Card, 5> &result)
    {
        sortCards(cards, cardValueAscAs1);

        size_t pos = 0;
        size_t number = 0;
        result[number++] = cards[pos++];
        while (pos < cards.size())
        {
            if (cards[pos - 1].getValue() - cards[pos].getValue(true) == 1)
            {
                result[number++] = cards[pos];
            }
            else
            {
                number = 0;
                result[number++] = cards[pos];
            }
            if (number == 5)
                break;
            ++pos;
        }

        return number == 5;
    }

    bool isFlush(std::vector<Card>& cards, 
        std::array<Card, 5> &result)
    {
        sortCards(cards, cardValue);
        for (size_t i = 0; i < 5; i++)
        {
            result[i] = result[i];
        }
        return true;
    }

    size_t processSuits(std::array<Card, 5> &result)
    {
        auto cards = getLargestSuit();

        if (cards.size() < 5) return unKnow;

        bool rb = false;

        rb = isRoyalFlush(cards, result);
        if(rb) return royalFlush;

        rb = isStraightFlush(cards, result);
        if(rb) return straightFlush;

        rb = isStraightFlush(cards, result);

        if(rb) return flush;
        return unKnow;
        
    }

    std::vector<std::vector<Card>> groupByMostSameValue()
    {
        auto sCards = cardCenter_.getCards();
        std::vector<Card> cards(sCards.begin(), sCards.end());
        sortCards(cards, cardValue);
        std::vector<std::vector<Card>> cardGroups;
        size_t i = 0;
        std::vector<Card> group;
        group.push_back(cards[i++]);
        for (; i < cards.size(); i++)
        {
            if(cards[i].getValue() != cards[i-1].getValue())
            {
                cardGroups.emplace_back(std::move(group));
                group.clear();
                group.shrink_to_fit();
            }
            group.push_back(cards[i]);
        }

        cardGroups.emplace_back(std::move(group));
        std::sort(cardGroups.begin(),cardGroups.end(), 
        [](const std::vector<Card>& g1, const std::vector<Card>& g2)
        {
            return g1.size() > g2.size();
        });
        return cardGroups;
    }

    uint8_t processSameValues(std::array<Card, 5> &result)
    {
        auto cards = groupByMostSameValue();
        assert(cards.at(0).size() <= 4);
        if(cards.at(0).size() == 4)
        {
            for (size_t i = 0; i < 4; i++)
            {
                result.at(i) = cards.at(0).at(i);
            }
            result.at(4) = cards.at(1).at(0);
            return fourOfAkind;
        }

        if(cards.at(0).size() == 3)
        {
            if(cards.at(1).size() > 1)
            {
                for (size_t i = 0; i < 3; i++)
                {
                    result.at(i) = cards.at(0).at(i);
                }
                for (size_t i = 0; i < 2; i++)
                {
                    result.at(i+3) = cards.at(1).at(i);
                }
                return fullHouse;
            }else
            {
                for (size_t i = 0; i < 3; i++)
                {
                    result.at(i) = cards.at(0).at(i);
                }
                result[3] = cards.at(1).at(0);
                result[4] = cards.at(2).at(0);
                return threeOfAkind;
            }
        }

        if(cards.at(0).size() == 2)
        {
            if(cards.at(1).size() == 2)
            {
                for (size_t i = 0; i < 2; i++)
                {
                    result.at(i) = cards.at(0).at(i);
                }
                for (size_t i = 0; i < 2; i++)
                {
                    result.at(i+2) = cards.at(1).at(i);
                }
                result[4] = cards.at(2).at(0);
                return twoPair;
            }else
            {
                assert(cards.size() == 6);
                for (size_t i = 0; i < 2; i++)
                {
                    result.at(i) = cards.at(0).at(i);
                }
                result[2] = cards.at(1).at(0);
                result[3] = cards.at(2).at(0);
                result[4] = cards.at(3).at(0);
                return onePair;
            }
        }

        assert(cards.size() == 7);

        for (size_t i = 0; i < 5; i++)
        {
            result.at(i) = cards.at(i).at(0);
        }

        return hignCard;
    }

    std::vector<Card> removeDuplicateValue(std::vector<Card>&& cards)
    {
        std::vector<Card> outCards;
        size_t i = 0;
        outCards.push_back(cards[i++]);
        for (; i < cards.size(); i++)
        {
            if(cards[i].getValue() != cards[i-1].getValue())
            {
                outCards.push_back(cards[i]);
            }
        }
        return outCards;
    }

    uint8_t processStraight(std::array<Card, 5> &result, 
        std::vector<Card>& cards,  bool ascAs1)
    {
        sortCards(cards, ascAs1 ? cardValueAscAs1 : cardValue);
        size_t i = 0;
        size_t number = 0;
        result[number++] = cards[i++];
        for (; i < cards.size(); i++)
        {
            if(cards[i-1].getValue(ascAs1) - cards[i].getValue(ascAs1) != 1)
            {
                number = 0;
            }
            result[number++] = cards[i];
            if(number == 5) break;
        }
        return (number == 5) ? straight : unKnow;
    }

    uint8_t processStraight(std::array<Card, 5> &result)
    {
        auto sCards = cardCenter_.getCards();
        std::vector<Card> cards(sCards.begin(), sCards.end());
        sortCards(cards,  cardValue);
        cards = removeDuplicateValue(std::move(cards));
        if(cards.size() < 5) return unKnow;

        auto model = processStraight(result, cards, false);
        if(model == unKnow) 
        {
            model = processStraight(result, cards, true);
        }
        
        return model;
    }

private:

    CardCenter<7> cardCenter_;
};
