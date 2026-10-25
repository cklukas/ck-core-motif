---
nav_exclude: true
---
# StackTransientWindow guards

Transient restacking can run after a parent or sibling has been removed.
`StackTransientWindow` now returns for a missing client or leader and stops
when no predecessor is found in the sibling chain.  Valid X stacking remains
unchanged.
