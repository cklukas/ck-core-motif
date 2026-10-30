---
nav_exclude: true
---

# Badge rendering input

Badge `Redisplay` now validates the widget class before reading label/count
state or issuing X drawing calls. Invalid expose dispatch returns safely;
normal Badge rendering is unchanged.

The focused source contract is checked by
`tests/automated/check-badge-redisplay-input.py`.
