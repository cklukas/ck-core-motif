---
nav_exclude: true
---

# Prompt text-length bounds

`XmPrompt` now bounds user-provided prompt text lengths before passing them to
`XDrawString`. Normal prompt rendering is unchanged; unusually long text no
longer wraps the signed Xlib length argument.

The focused regression check is
`tests/automated/check-prompt-text-bounds.py`.
