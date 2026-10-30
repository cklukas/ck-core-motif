---
nav_exclude: true
---
# FontSelector any-spacing callback guards

The any-spacing callback now validates callback data and current font state
before touching sibling toggles or refreshing the font. Valid selections retain
the existing fixed/proportional state and display update behavior.
