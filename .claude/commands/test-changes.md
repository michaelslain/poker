---
description: Write and run tests for all changes since last commit
---

Write comprehensive unit tests for all changes made since the last git commit:

1. **Identify changed files** - use git diff to find modified .cpp/.hpp files
2. **For each changed class**, write tests in the corresponding `tests/test_*.cpp` file:
   - If test file doesn't exist, create it following the pattern in CLAUDE.md
   - Add new TEST_CASE blocks for new functionality
   - Include regression tests for any bug fixes
   - Test edge cases and boundary conditions
   - Use appropriate Catch2 assertions (REQUIRE, CHECK, SECTION)
3. **Test organization**:
   - Use descriptive test names: `TEST_CASE("ClassName - Feature", "[tag]")`
   - Group related tests in SECTIONs
   - Tag tests appropriately ([regression], [physics], [inventory], etc.)
4. **Ensure tests compile and pass**:
   - Add new test files to TEST_SRCS in Makefile if needed
   - Run `make test` to compile and execute all tests
   - Fix any compilation or runtime errors
   - Verify all tests pass

Provide a summary of:
- Which classes were tested
- How many new tests were added
- Test coverage for new functionality
- Any tests that failed and why
