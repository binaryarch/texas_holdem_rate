#pragma once
#include <cstdint>
#include <cassert>
#include <array>
#include <bitset>
#include <string_view>

enum Suits : uint8_t
{
    spades,
    earts,
    diamonds,
    clubs
};

constexpr static std::array<std::string_view, 13> cardSuits =
    {
        "♠", "♡", "♣", "♢"};

constexpr static std::array<char, 14> cardChars =
    {
        'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};

static inline size_t getCardCharIndex(char value)
{
    const auto sz = cardChars.size();
    size_t i = 0;
    for (; i < sz; i++)
    {
        if (cardChars[i] == value)
            break;
    }
    assert(i != sz);
    return static_cast<uint8_t>(i);
}

class Card
{
public:
    Card():index_(52){}
    Card(uint8_t index) : index_(index)
    {
        assert(index_ / 13 <= clubs);
    }

    Card(uint8_t suits, char value) : index_(suits * 13 + getCardCharIndex(value))
    {
        assert(suits <= clubs);
    }

    uint8_t getValue(bool aceAs1 = false) const
    {
        auto val = index_ % 13;
        return (val != 0 || aceAs1) ? val : 13;
    }

    uint8_t getSuit() const
    {
        return index_ / 13;
    }

    uint8_t getIndex() const
    {
        return index_;
    }

    friend std::ostream &operator<<(std::ostream &os, const Card &card)
    {
        return os << cardSuits[card.getSuit()] << cardChars[card.getValue(true)];
    }

    bool operator==(const Card &other) const
    {
        return getValue() == other.getValue();
    }

    bool operator>(const Card &other) const
    {
        return getValue() > other.getValue();
    }

    bool operator<(const Card &other) const
    {
        return getValue() < other.getValue();
    }

    int compare(const Card &other) const
    {
        if (*this > other)
            return 1;
        if (*this < other)
            return -1;
        return 0;
    }

private:
    uint8_t index_;
};

template<size_t S>
class CardCenter
{
public:
    CardCenter() = default;
    CardCenter(const CardCenter& ) = default;
    CardCenter& operator=(const CardCenter& ) = default;

    CardCenter(const std::array<Card, S>& card) :cards_(card)
    {
    }


    CardCenter(std::initializer_list<uint8_t> l)
    {
        assert(l.size() == S);
        size_t i = 0;
        for(auto it = l.begin(); it!=l.end(); ++it)
        {
            cards_[i++] = *it;
        }
    }




    int compare(const CardCenter& otehr)
    {
        for (size_t i = 0; i < S; i++)
        {
            if(cards_[i].getValue() > 
                otehr.cards_[i].getValue())
            {
                return 1;
            }else if(cards_[i].getValue() < 
                otehr.cards_[i].getValue())
            {
                return -1;
            }
        }
        return 0;
    }
    
    template<size_t U>
    CardCenter<S+U> operator+(const CardCenter<U>& other) const
    {
        CardCenter<S+U> cards;
        for (size_t i = 0; i < S; i++)
        {
            cards[i] = cards_[i];
        }
        for (size_t i = 0; i < U; i++)
        {
            cards[i+S] = other.getCards()[i];
        }
        return cards;
    }
    
    operator uint64_t() const
    {
        uint64_t u64Value{0};
        // |--------|--------|--------|--------|--------|--------|--------|--------|
        // |  card1    card2    card3    card4    card5    card6   card7  |cn(3)| 
        for (size_t i = 0; i < S; i++)
        {
            auto idx = cards_[i].getIndex();
            u64Value |= (idx << i*8);
        }
        u64Value |=(S << 7*8);  //3 bit

        return u64Value;
    }

    Card& operator[](size_t i)
    {
        return cards_.at(i);
    }

    std::array<Card, S> getCards() const
    {
        return cards_;
    }

    bool isVaild()
    {
        for (size_t i = 0; i < S; i++)
        {
            if(cards_[i].getIndex() == 52) return false;
        }
        return true;
    }
private:
    std::array<Card, S> cards_;
};

using HandCard = CardCenter<2>;
using FlopCard = CardCenter<3>;
using TurnCard = CardCenter<1>;
using RiverCard = CardCenter<1>;
using ResultCard = CardCenter<5>;



static std::array<Card, 52> allCards =
    {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13,
        14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
        27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
        40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51};

static std::bitset<52> allCardBits;