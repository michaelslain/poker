---
description: Review all changes since last commit for issues and architecture violations
---

Review all changes since the last git commit and check for:

1. **Obvious bugs or logic errors** - null pointer dereferences, off-by-one errors, uninitialized variables, memory leaks, etc.
2. **Architecture violations** - ensure code follows the OOP patterns in CLAUDE.md:
   - Proper inheritance hierarchy (Object → Interactable → Item, etc.)
   - Virtual functions marked with `override`
   - RAII pattern (constructors initialize, destructors clean up)
   - DOM-based object management (no objects rendering other objects directly)
   - Memory management (proper new/delete, no orphaned objects)
   - Physics integration (proper RigidBody usage, collision categories)
   - Lighting system (usesLighting flags, proper shader usage)
3. **Code quality issues**:
   - Inconsistent naming conventions
   - Missing null checks
   - Improper use of C++ features (should use std::vector not malloc, std::string not char*, etc.)
   - Texture/shader resource leaks
   - Missing virtual destructors where needed
4. **Common pitfalls from CLAUDE.md**:
   - Card ownership violations (only Deck should delete cards)
   - Spawner usage (they auto-spawn on construction)
   - Shader return types (must return references not copies)
   - Lighting initialization order
   - Enum syntax (C-style not C++11 style)

Provide a summary of findings organized by severity (critical, moderate, minor). If no issues found, confirm the code looks good.
