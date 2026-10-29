---
nav_exclude: true
---
# Sidebar child allocation

Sidebar creates a content row-column and a toggle button during initialization.
Either child may be unavailable after an allocation failure. Initialization and
later relayout now guard management, callbacks, preferred-size queries, and
geometry configuration so the widget remains safe and can recover on a later
layout pass.

The contract is protected by
`tests/automated/check-sidebar-child-failure-guards.py`.
