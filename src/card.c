#include "card.h"
#include "rlgl.h"
#include "raymath.h"
#include <stdio.h>

// Virtual function wrappers
static void Card_UpdateVirtual(Object* self, float deltaTime) {
    (void)deltaTime;
    Card_Update((Card*)self);
}

static void Card_DrawVirtual(Object* self, Camera3D camera) {
    Card_Draw((Card*)self, camera);
}

void Card_Init(Card* card, Suit suit, Rank rank, Vector3 pos, InteractCallback callback, PhysicsWorld* physics) {
    // Initialize the base item
    Item_Init(&card->base, pos);
    
    // Set the virtual function pointers
    card->base.base.base.getType = Card_GetType;
    card->base.base.base.update = Card_UpdateVirtual;
    card->base.base.base.draw = Card_DrawVirtual;
    
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
    
    // Initialize physics (card dimensions)
    Vector3 cardSize = { 0.5f, 0.7f, 0.02f };
    RigidBody_InitBox(&card->rigidBody, physics, pos, cardSize, 0.05f);  // Light mass for cards
}

void Card_Update(Card* card) {
    if (!card->base.base.isActive) return;
    
    // Sync the object position with physics body
    RigidBody_Update(&card->rigidBody);
    card->base.base.base.position = card->rigidBody.base.position;
    card->base.base.base.rotation = card->rigidBody.base.rotation;
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

void Card_Draw(Card* card, Camera3D camera) {
    (void)camera;
    if (!card->base.base.isActive) return;
    
    Vector3 pos = card->base.base.base.position;
    
    // Card dimensions
    float cardWidth = 0.5f;
    float cardHeight = 0.7f;
    float cardThickness = 0.02f;
    
    // Get rotation matrix from physics body
    Matrix rotMatrix = RigidBody_GetRotationMatrix(&card->rigidBody);
    Matrix transMatrix = MatrixTranslate(pos.x, pos.y, pos.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
    
    // Draw white card rectangle with physics rotation
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        
        DrawCube((Vector3){0, 0, 0}, cardWidth, cardHeight, cardThickness, WHITE);
        DrawCubeWires((Vector3){0, 0, 0}, cardWidth, cardHeight, cardThickness, DARKGRAY);
        
        // Draw the texture on the front face
        rlTranslatef(0, 0, cardThickness/2 + 0.01f);
        
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
    RigidBody_Cleanup(&card->rigidBody);
}

const char* Card_GetType(Object* obj) {
    Card* card = (Card*)obj;
    static char typeBuffer[64];
    
    // Get suit name (lowercase)
    const char* suitName;
    switch (card->suit) {
        case SUIT_HEARTS:   suitName = "hearts"; break;
        case SUIT_DIAMONDS: suitName = "diamonds"; break;
        case SUIT_CLUBS:    suitName = "clubs"; break;
        case SUIT_SPADES:   suitName = "spades"; break;
        default:            suitName = "unknown"; break;
    }
    
    // Get rank name (lowercase)
    const char* rankName;
    switch (card->rank) {
        case RANK_ACE:   rankName = "ace"; break;
        case RANK_TWO:   rankName = "two"; break;
        case RANK_THREE: rankName = "three"; break;
        case RANK_FOUR:  rankName = "four"; break;
        case RANK_FIVE:  rankName = "five"; break;
        case RANK_SIX:   rankName = "six"; break;
        case RANK_SEVEN: rankName = "seven"; break;
        case RANK_EIGHT: rankName = "eight"; break;
        case RANK_NINE:  rankName = "nine"; break;
        case RANK_TEN:   rankName = "ten"; break;
        case RANK_JACK:  rankName = "jack"; break;
        case RANK_QUEEN: rankName = "queen"; break;
        case RANK_KING:  rankName = "king"; break;
        default:         rankName = "unknown"; break;
    }
    
    // Format: "card_suit_rank"
    snprintf(typeBuffer, sizeof(typeBuffer), "card_%s_%s", suitName, rankName);
    return typeBuffer;
}
