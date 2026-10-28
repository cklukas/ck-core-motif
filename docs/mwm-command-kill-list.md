---
nav_exclude: true
---
# MWM command-removal list capacity

When MWM removes commands belonging to a client, matching command IDs are
collected in a fixed internal array. The collector now checks that array's
capacity before appending an ID and reports when an excess entry is skipped,
preventing a large command tree from overwriting adjacent state.
