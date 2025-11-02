#ifndef POKER_TABLE_HPP
#define POKER_TABLE_HPP

#include "interactable.hpp"
#include "deck.hpp"
#include "physics.hpp"
#include <ode/ode.h>
#include <array>

#define MAX_PLAYERS 8  // Maximum players at a poker table

// Forward declaration
class Player;

class PokerTable : public Interactable {
private:
    std::array<Player*, MAX_PLAYERS> players;
    int playerCount;
    Deck deck;
    Vector3 size;
    Color color;
    dGeomID geom;
    PhysicsWorld* physics;

public:
    PokerTable(Vector3 pos, Vector3 tableSize, Color tableColor, PhysicsWorld* physicsWorld);
    virtual ~PokerTable();
    
    // Override virtual functions
    void Update(float deltaTime) override;
    void Draw(Camera3D camera) override;
    void Interact() override;
    const char* GetType() const override;
    
    // Table-specific methods
    bool AddPlayer(Player* player);
    void RemovePlayer(Player* player);
    bool HasPlayer(Player* player);
    void InteractWithPlayer(Player* player);
    
    // Accessors
    int GetPlayerCount() const { return playerCount; }
    Deck* GetDeck() { return &deck; }
    dGeomID GetGeom() const { return geom; }
};

#endif
