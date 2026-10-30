---
nav_exclude: true
---

# Avatar initialization input

Avatar initialization now validates the widget class before copying initials,
loading fonts, or creating its graphics context. Malformed lifecycle calls
return safely; normal Avatar construction remains unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-initialize-input.py`.
