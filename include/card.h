#ifndef CARD_H
#define CARD_H

#include "interactable.h"

typedef enum {
    SUIT_HEARTS,
    SUIT_DIAMONDS,
    SUIT_CLUBS,
    SUIT_SPADES
} Suit;

typedef enum {
    RANK_ACE = 1,
    RANK_TWO,
    RANK_THREE,
    RANK_FOUR,
    RANK_FIVE,
    RANK_SIX,
    RANK_SEVEN,
    RANK_EIGHT,
    RANK_NINE,
    RANK_TEN,
    RANK_JACK,
    RANK_QUEEN,
    RANK_KING
} Rank;

typedef struct {
    Interactable base;
    Suit suit;
    Rank rank;
    RenderTexture2D texture;
    bool textureLoaded;
} Card;

void Card_Init(Card* card, Suit suit, Rank rank, Vector3 pos, InteractCallback callback);
void Card_Draw(Card* card, bool isClosest, Camera3D camera);
void Card_Cleanup(Card* card);
const char* Card_GetSuitSymbol(Suit suit);
const char* Card_GetRankString(Rank rank);
Color Card_GetSuitColor(Suit suit);

#endif
