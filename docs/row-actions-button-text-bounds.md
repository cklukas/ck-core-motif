---
nav_exclude: true
---

# Row-actions button-text allocation bounds

`XmRowActions` assembles an action label and optional shortcut directly as
compound `XmString` segments. This avoids flattening multilingual labels into
a temporary byte buffer and preserves charset information in the visible
button.

The focused regression check is
`tests/automated/check-row-actions-button-text-bounds.py`.
