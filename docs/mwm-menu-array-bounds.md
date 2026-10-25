---
nav_exclude: true
---

# MWM menu query array bounds

MWM menu-query replies use fixed arrays sized by `MAX_MENU_ITEMS`. The icon,
window-item, and item-check queries now cap their source-menu traversals at
that same capacity, matching their bounded packing loops. Menus larger than
the protocol capacity are truncated in the reply instead of writing past stack
arrays.

The focused regression check is
`tests/automated/check-mwm-menu-array-bounds.py`.
