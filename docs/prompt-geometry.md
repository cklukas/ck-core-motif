---
nav_exclude: true
---
# XmPrompt geometry

`XmPrompt` now reports a preferred height based on its XmString render-table
metrics, with the same padding used during initialization. This prevents
larger fonts and complex-script text from being clipped by the old fixed
24-pixel geometry; the 24-pixel default remains for an empty prompt.

The focused source contract is checked by
`tests/automated/check-prompt-geometry.py`.
