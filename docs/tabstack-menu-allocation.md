---
nav_exclude: true
---
# TabStack menu construction

The TabStack overflow menu creates push-button children for valid tabs. Menu
item creation can fail under memory pressure; the builder now stops cleanly,
guards callback registration, and checks the refreshed child count before
assigning labels or managing a button. Existing valid menu entries remain
usable and no child-array overrun is possible.

The contract is protected by
`tests/automated/check-tabstack-menu-allocation.py`.
