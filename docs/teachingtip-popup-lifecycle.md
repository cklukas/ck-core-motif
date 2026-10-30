---
nav_exclude: true
---
# TeachingTip popup lifecycle

TeachingTip checks its transient shell, content container, and both label
children before using them.  Allocation failure destroys the partial popup and
returns cleanly.

The popup shell also clears all stored widget pointers and the visible state
when destroyed externally, so the next show can build a fresh tip.
