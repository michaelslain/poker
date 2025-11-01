#include "deck.h"
#include <stdlib.h>
#include <time.h>

void Deck_Init(Deck* deck) {
    deck->count = 0;
    
    // Generate all 52 cards
    int index = 0;
    for (int suit = SUIT_HEARTS; suit <= SUIT_SPADES; suit++) {
        for (int rank = RANK_ACE; rank <= RANK_KING; rank++) {
            // Allocate card
            Card* card = (Card*)malloc(sizeof(Card));
            
            // Initialize card at origin with no physics (NULL physics world)
            // Caller will set position when they use the card
            Card_Init(card, (Suit)suit, (Rank)rank, (Vector3){0, 0, 0}, NULL, NULL);
            
            deck->cards[index++] = card;
        }
    }
    
    deck->count = DECK_SIZE;
}

void Deck_Shuffle(Deck* deck) {
    if (deck->count <= 1) return;
    
    // Fisher-Yates shuffle algorithm
    for (int i = deck->count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        
        // Swap cards[i] and cards[j]
        Card* temp = deck->cards[i];
        deck->cards[i] = deck->cards[j];
        deck->cards[j] = temp;
    }
}

Card* Deck_Draw(Deck* deck) {
    if (deck->count <= 0) {
        return NULL;
    }
    
    // Draw from the "top" (end of array)
    deck->count--;
    return deck->cards[deck->count];
}

Card* Deck_Peek(Deck* deck) {
    if (deck->count <= 0) {
        return NULL;
    }
    
    return deck->cards[deck->count - 1];
}

void Deck_Reset(Deck* deck) {
    // Reset count to full deck (doesn't recreate cards)
    deck->count = DECK_SIZE;
}

void Deck_Cleanup(Deck* deck) {
    // Cleanup and free all cards
    for (int i = 0; i < DECK_SIZE; i++) {
        if (deck->cards[i]) {
            Card_Cleanup(deck->cards[i]);
            free(deck->cards[i]);
            deck->cards[i] = NULL;
        }
    }
    deck->count = 0;
}
