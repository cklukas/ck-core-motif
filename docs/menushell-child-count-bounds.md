---
nav_exclude: true
---
# MenuShell child-count bounds

MenuShell expose, popup-child lookup, and managed-child focus traversal now
use Xt `Cardinal` indices for composite child arrays. This preserves menu
ordering and focus behavior while avoiding signed-index wraparound for large
menu hierarchies.
