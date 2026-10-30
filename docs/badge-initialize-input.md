---
nav_exclude: true
---

# Badge initialization input

Badge initialization now validates the widget class before copying labels,
fonts, or creating its drawing context. Malformed lifecycle calls return
safely; normal Badge construction is unchanged.

The focused source contract is checked by
`tests/automated/check-badge-initialize-input.py`.
