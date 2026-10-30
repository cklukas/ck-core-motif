---
nav_exclude: true
---

# Avatar teardown input

Avatar destruction now validates the widget class before freeing its graphics
context, fallback font, and render table. Invalid teardown dispatch returns
safely; normal Avatar destruction is unchanged.

The focused source contract is checked by
`tests/automated/check-avatar-destroy-input.py`.
