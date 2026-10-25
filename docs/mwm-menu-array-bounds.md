---
nav_exclude: true
---

# MWM menu query array bounds

MWM menu-query replies use fixed arrays sized by `MAX_MENU_ITEMS`. The icon,
window-item, and item-check queries now cap their source-menu traversals at
that same capacity, matching their bounded packing loops. The reported item
count is capped with the packed entries, so clients do not parse beyond the
reply. Menus larger than the protocol capacity are truncated safely.

The focused regression check is
`tests/automated/check-mwm-menu-array-bounds.py`.
