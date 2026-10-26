---
nav_exclude: true
---
# XmPrompt live color updates

`XmPrompt` now updates its drawing GC when foreground or background resources
change through `SetValues`, and requests a repaint for that transition. Live
theme or resource changes therefore take effect immediately instead of leaving
stale text colors in an already-created prompt.

The focused source contract is checked by
`tests/automated/check-prompt-color-updates.py`.
