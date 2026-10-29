---
nav_exclude: true
---
# EmptyState child allocation

EmptyState creates optional icon, message, and action children.  Initialization,
layout, and `SetValues` now guard each child before querying geometry, changing
resources, registering callbacks, or configuring it.  A failed internal child
creation therefore cannot turn a later redraw or resource update into a null
widget call.

The contract is protected by
`tests/automated/check-empty-state-child-failure-guards.py`.
