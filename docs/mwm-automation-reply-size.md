---
nav_exclude: true
---

# MWM automation reply sizing

`GetAutomationData` builds replies for the MWM automation protocol.  Menu
items include variable-length labels, so the byte count must be able to hold
the result of `strlen()` plus the fixed protocol fields.  The counter now uses
`size_t`, avoiding truncation on platforms where a valid reply exceeds the
range of a signed `int` while leaving the wire format unchanged.

The existing `MAX_MENU_ITEMS` cap remains in force.  It limits the number of
items serialized, while this change keeps the byte arithmetic accurate for
the labels that are serialized.
