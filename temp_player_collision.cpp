// New collision detection and sliding code

// Helper to get geometry from any object type
auto getGeomFromObject = [](Object* obj) -> dGeomID {
    std::string typeStr = obj->GetType();
    if (typeStr == "poker_table") {
        return static_cast<PokerTable*>(obj)->GetGeom();
    } else if (typeStr == "wall") {
        return static_cast<Wall*>(obj)->GetGeom();
    }
    // Add more collider types here as needed
    return nullptr;
};

// Check for collisions
Vector3 finalPos = newPos;
bool foundCollision = false;
DOM* dom = DOM::GetGlobal();

// Iterative sliding - try up to 3 iterations to handle corners
for (int iteration = 0; iteration < 3 && dom; iteration++) {
    dGeomSetPosition(geom, finalPos.x, finalPos.y + 0.85f, finalPos.z);
    
    Vector3 collisionNormal = {0, 0, 0};
    bool collided = false;
    
    // Check all objects for collisions
    for (int i = 0; i < dom->GetCount(); i++) {
        Object* obj = dom->GetObject(i);
        dGeomID otherGeom = getGeomFromObject(obj);
        
        if (otherGeom != nullptr) {
            dContactGeom contacts[4];
            int numContacts = dCollide(geom, otherGeom, 4, contacts, sizeof(dContactGeom));
            if (numContacts > 0) {
                collided = true;
                foundCollision = true;
                collisionNormal.x = contacts[0].normal[0];
                collisionNormal.y = contacts[0].normal[1];
                collisionNormal.z = contacts[0].normal[2];
                break;
            }
        }
    }
    
    if (!collided) {
        // No collision, we're done
        break;
    }
    
    // Collision detected - try to slide
    Vector3 moveVec = Vector3Subtract(finalPos, oldPos);
    float dotProduct = Vector3DotProduct(moveVec, collisionNormal);
    
    if (dotProduct < 0) {
        // Slide along the surface
        Vector3 slideDir = Vector3Subtract(moveVec, Vector3Scale(collisionNormal, dotProduct));
        finalPos = Vector3Add(oldPos, slideDir);
    } else {
        // Moving away from surface, stop here
        finalPos = oldPos;
        break;
    }
}

// Apply final position
if (foundCollision) {
    dGeomSetPosition(geom, finalPos.x, finalPos.y + 0.85f, finalPos.z);
    dBodySetPosition(body, finalPos.x, finalPos.y + 0.85f, finalPos.z);
    position = finalPos;
} else {
    dBodySetPosition(body, finalPos.x, finalPos.y + 0.85f, finalPos.z);
    position = finalPos;
}
