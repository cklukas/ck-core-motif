---
nav_exclude: true
---

# Avatar rendering input

Avatar `Redisplay` now validates the widget class before retrieving display and
window state or drawing. Invalid expose dispatch returns safely; normal Avatar
rendering remains unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-redisplay-input.py`.
