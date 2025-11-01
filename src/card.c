#include "card.h"
#include "render_utils.h"
#include "rlgl.h"
#include <stdio.h>

void Card_Init(Card* card, Suit suit, Rank rank, Vector3 pos, InteractCallback callback) {
    // Initialize the base item
    Item_Init(&card->base, pos);
    
    // Override callback if provided
    if (callback) {
        card->base.base.onInteract = callback;
    }
    
    // Set card-specific properties
    card->suit = suit;
    card->rank = rank;
    
    // Create texture for this card
    card->texture = LoadRenderTexture(256, 356);  // Card aspect ratio
    card->textureLoaded = true;
    
    // Render the card face to the texture
    const char* suitSymbol = Card_GetSuitSymbol(suit);
    const char* rankStr = Card_GetRankString(rank);
    Color textColor = Card_GetSuitColor(suit);
    
    BeginTextureMode(card->texture);
        ClearBackground(WHITE);
        
        // Draw rank in center (larger)
        int rankWidth = MeasureText(rankStr, 60);
        DrawText(rankStr, 128 - rankWidth/2, 100, 60, textColor);
        
        // Draw suit below rank (smaller)
        int suitWidth = MeasureText(suitSymbol, 30);
        DrawText(suitSymbol, 128 - suitWidth/2, 200, 30, textColor);
    EndTextureMode();
}

const char* Card_GetSuitSymbol(Suit suit) {
    switch (suit) {
        case SUIT_HEARTS:   return "HEARTS";
        case SUIT_DIAMONDS: return "DIAMONDS";
        case SUIT_CLUBS:    return "CLUBS";
        case SUIT_SPADES:   return "SPADES";
        default:            return "?";
    }
}

const char* Card_GetRankString(Rank rank) {
    static char buffer[3];
    switch (rank) {
        case RANK_ACE:   return "A";
        case RANK_JACK:  return "J";
        case RANK_QUEEN: return "Q";
        case RANK_KING:  return "K";
        default:
            snprintf(buffer, sizeof(buffer), "%d", rank);
            return buffer;
    }
}

Color Card_GetSuitColor(Suit suit) {
    switch (suit) {
        case SUIT_HEARTS:
        case SUIT_DIAMONDS:
            return RED;
        case SUIT_CLUBS:
        case SUIT_SPADES:
            return BLACK;
        default:
            return BLACK;
    }
}

void Card_Draw(Card* card, bool isClosest, Camera3D camera) {
    (void)camera; // We're not using billboard anymore
    if (!card->base.base.isActive) return;
    
    Vector3 pos = card->base.base.base.position;
    
    // Card dimensions
    float cardWidth = 0.5f;
    float cardHeight = 0.7f;
    float cardThickness = 0.02f;
    
    // Draw white card rectangle
    Color outlineColor = isClosest ? YELLOW : DARKGRAY;
    DrawCube(pos, cardWidth, cardHeight, cardThickness, WHITE);
    DrawCubeWires(pos, cardWidth, cardHeight, cardThickness, outlineColor);
    
    // Draw the texture on the front face as a simple plane facing forward (positive Z)
    rlPushMatrix();
        rlTranslatef(pos.x, pos.y, pos.z + cardThickness/2 + 0.01f);
        
        rlSetTexture(card->texture.texture.id);
        rlBegin(RL_QUADS);
            rlColor4ub(255, 255, 255, 255);
            rlNormal3f(0.0f, 0.0f, 1.0f);
            
            // Draw quad facing forward (normal coords, no flip needed)
            rlTexCoord2f(0.0f, 0.0f); rlVertex3f(-cardWidth/2, -cardHeight/2, 0.0f);
            rlTexCoord2f(1.0f, 0.0f); rlVertex3f(cardWidth/2, -cardHeight/2, 0.0f);
            rlTexCoord2f(1.0f, 1.0f); rlVertex3f(cardWidth/2, cardHeight/2, 0.0f);
            rlTexCoord2f(0.0f, 1.0f); rlVertex3f(-cardWidth/2, cardHeight/2, 0.0f);
        rlEnd();
        rlSetTexture(0);
    rlPopMatrix();
}

void Card_DrawIcon(Card* card, Rectangle destRect) {
    if (!card->textureLoaded) return;
    
    // Draw the card's texture as a flat 2D icon (flip vertically with negative height)
    Rectangle sourceRec = { 0, 0, (float)card->texture.texture.width, -(float)card->texture.texture.height };
    DrawTexturePro(card->texture.texture, sourceRec, destRect, (Vector2){0, 0}, 0.0f, WHITE);
}

void Card_Cleanup(Card* card) {
    if (card->textureLoaded) {
        UnloadRenderTexture(card->texture);
        card->textureLoaded = false;
    }
}
