---
nav_exclude: true
---
# FontSelector fixed-spacing callback guards

The fixed-spacing callback now validates callback data and current font state
before touching sibling toggles or refreshing the font. Valid selections retain
the existing fixed/proportional state and display update behavior.
