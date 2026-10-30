---
nav_exclude: true
---
# FontSelector proportional callback guards

The proportional-spacing callback now validates callback data and current font
state before touching sibling toggles or refreshing the font. Valid selections
retain the existing family, size, and display update behavior.
