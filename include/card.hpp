#ifndef CARD_HPP
#define CARD_HPP

#include "item.hpp"
#include "rigidbody.hpp"
#include "physics.hpp"

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

class Card : public Item {
public:
    Suit suit;
    Rank rank;
    RenderTexture2D texture;
    bool textureLoaded;
    RigidBody* rigidBody;
    Model model;
    bool isClosestInteractable;

    Card(Suit s, Rank r, Vector3 pos = {0.0f, 0.0f, 0.0f}, PhysicsWorld* physics = nullptr);
    virtual ~Card();

    void AttachPhysics(Vector3 pos, PhysicsWorld* physics);
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    void DrawIcon(Rectangle destRect) override;
    std::string GetType() const override;
    
    static const char* GetSuitSymbol(Suit s);
    static const char* GetRankString(Rank r);
    static Color GetSuitColor(Suit s);
};

#endif
