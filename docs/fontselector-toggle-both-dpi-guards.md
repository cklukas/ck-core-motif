---
nav_exclude: true
---
# FontSelector both-DPI callback guards

The both-DPI callback now validates callback data and current font state before
touching sibling toggles or resolution fields. Valid selections retain the
existing “any resolution” family, size, and display refresh behavior.
