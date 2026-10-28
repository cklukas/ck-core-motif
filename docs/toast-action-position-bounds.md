---
nav_exclude: true
---
# Toast action-position bounds

`XmToast` places its optional action button from the toast width and the
button width. The old direct cast to Motif `Position` could wrap the action
origin on unusually wide layouts.

The origin now uses a signed wide calculation and clamps to the representable
Position range. Normal toast placement is unchanged.

The focused source regression is
`tests/automated/check-toast-action-position-bounds.py`.
