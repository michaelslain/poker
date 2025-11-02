#include "card.hpp"
#include "rlgl.h"
#include "raymath.h"
#include <cstdio>

Card::Card(Suit s, Rank r, Vector3 pos, PhysicsWorld* physics)
    : Item(pos), suit(s), rank(r), textureLoaded(false), rigidBody(nullptr)
{
    // Create texture for this card
    texture = LoadRenderTexture(256, 356);  // Card aspect ratio
    textureLoaded = true;
    
    // Render the card face to the texture
    const char* suitSymbol = GetSuitSymbol(suit);
    const char* rankStr = GetRankString(rank);
    Color textColor = GetSuitColor(suit);
    
    BeginTextureMode(texture);
        ClearBackground(WHITE);
        
        // Draw rank in center (larger)
        int rankWidth = MeasureText(rankStr, 60);
        DrawText(rankStr, 128 - rankWidth/2, 100, 60, textColor);
        
        // Draw suit below rank (smaller)
        int suitWidth = MeasureText(suitSymbol, 30);
        DrawText(suitSymbol, 128 - suitWidth/2, 200, 30, textColor);
    EndTextureMode();
    
    // Initialize physics if provided
    if (physics) {
        AttachPhysics(pos, physics);
    }
}

Card::~Card() {
    if (textureLoaded) {
        UnloadRenderTexture(texture);
        textureLoaded = false;
    }
    if (rigidBody) {
        delete rigidBody;
        rigidBody = nullptr;
    }
}

void Card::AttachPhysics(Vector3 pos, PhysicsWorld* physics) {
    position = pos;
    
    // Initialize physics (card dimensions)
    Vector3 cardSize = { 0.5f, 0.7f, 0.02f };
    rigidBody = new RigidBody(pos);
    rigidBody->InitBox(physics, pos, cardSize, 0.05f);  // Light mass for cards
}

void Card::Update(float deltaTime) {
    (void)deltaTime;
    if (!isActive) return;
    
    // Only sync if physics is attached
    if (rigidBody && rigidBody->body) {
        rigidBody->Update(deltaTime);
        position = rigidBody->position;
        rotation = rigidBody->rotation;
    }
}

void Card::Draw(Camera3D camera) {
    (void)camera;
    if (!isActive) return;
    
    // Card dimensions
    float cardWidth = 0.5f;
    float cardHeight = 0.7f;
    float cardThickness = 0.02f;
    
    // Get rotation matrix from physics body
    Matrix rotMatrix = rigidBody ? rigidBody->GetRotationMatrix() : MatrixIdentity();
    Matrix transMatrix = MatrixTranslate(position.x, position.y, position.z);
    Matrix transform = MatrixMultiply(rotMatrix, transMatrix);
    
    // Draw white card rectangle with physics rotation
    rlPushMatrix();
        rlMultMatrixf(MatrixToFloat(transform));
        
        DrawCube({0, 0, 0}, cardWidth, cardHeight, cardThickness, WHITE);
        DrawCubeWires({0, 0, 0}, cardWidth, cardHeight, cardThickness, DARKGRAY);
        
        // Draw the texture on the front face
        rlTranslatef(0, 0, cardThickness/2 + 0.01f);
        
        rlSetTexture(texture.texture.id);
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

void Card::DrawIcon(Rectangle destRect) {
    if (!textureLoaded) return;
    
    // Draw the card's texture as a flat 2D icon (flip vertically with negative height)
    Rectangle sourceRec = { 0, 0, (float)texture.texture.width, -(float)texture.texture.height };
    DrawTexturePro(texture.texture, sourceRec, destRect, {0, 0}, 0.0f, WHITE);
}

const char* Card::GetType() const {
    static char typeBuffer[64];
    
    // Get suit name (lowercase)
    const char* suitName;
    switch (suit) {
        case SUIT_HEARTS:   suitName = "hearts"; break;
        case SUIT_DIAMONDS: suitName = "diamonds"; break;
        case SUIT_CLUBS:    suitName = "clubs"; break;
        case SUIT_SPADES:   suitName = "spades"; break;
        default:            suitName = "unknown"; break;
    }
    
    // Get rank name (lowercase)
    const char* rankName;
    switch (rank) {
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

const char* Card::GetSuitSymbol(Suit s) {
    switch (s) {
        case SUIT_HEARTS:   return "HEARTS";
        case SUIT_DIAMONDS: return "DIAMONDS";
        case SUIT_CLUBS:    return "CLUBS";
        case SUIT_SPADES:   return "SPADES";
        default:            return "?";
    }
}

const char* Card::GetRankString(Rank r) {
    static char buffer[3];
    switch (r) {
        case RANK_ACE:   return "A";
        case RANK_JACK:  return "J";
        case RANK_QUEEN: return "Q";
        case RANK_KING:  return "K";
        default:
            snprintf(buffer, sizeof(buffer), "%d", r);
            return buffer;
    }
}

Color Card::GetSuitColor(Suit s) {
    switch (s) {
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
