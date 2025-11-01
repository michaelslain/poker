#ifndef DECK_H
#define DECK_H

#include "card.h"

#define DECK_SIZE 52

typedef struct {
    Card* cards[DECK_SIZE];  // Array of card pointers
    int count;               // Number of cards remaining in deck
} Deck;

// Initialize deck with all 52 cards (allocates and initializes cards)
// Cards are NOT added to DOM - caller must add them manually if desired
// Cards are created without physics (physics=NULL)
void Deck_Init(Deck* deck);

// Shuffle the deck randomly
void Deck_Shuffle(Deck* deck);

// Draw a card from the top of the deck
// Returns NULL if deck is empty
// Removes the card from the deck (decrements count)
Card* Deck_Draw(Deck* deck);

// Peek at the top card without removing it
// Returns NULL if deck is empty
Card* Deck_Peek(Deck* deck);

// Reset the deck to full 52 cards (doesn't reallocate, just resets count)
void Deck_Reset(Deck* deck);

// Cleanup all cards in the deck
void Deck_Cleanup(Deck* deck);

#endif // DECK_H
