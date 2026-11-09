---
nav_exclude: true
---

# MWM automation menu-query initialization

MWM's fixed-format menu automation replies retain one slot for each advertised
menu button, including a button that is currently unmanaged. Those placeholder
slots now have deterministic neutral fields: an empty label, insensitive state,
and a zero vertical position. Consumers can therefore retain the established
reply count and indexing without observing stack data for an unmanaged button.

The item-check reply likewise initializes every title-gadget presence flag
before scanning the frame. If a decoration is absent, its existing INVALID
sentinel is now selected deterministically.

The focused regression check is
`tests/automated/check-mwm-menu-query-initialization.py`.
