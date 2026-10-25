---
nav_exclude: true
---

# WSM configuration rollback pointers

When a screen's WSM configuration is cleared, each attribute array is freed
and its count reset.  The rollback now also nulls the corresponding pointer,
so later callers cannot observe a dangling array through the configuration
record even while the count is zero.
