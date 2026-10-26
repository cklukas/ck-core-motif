---
nav_exclude: true
---
# ProgressRing geometry with an empty label

ProgressRing geometry negotiation now measures label metrics only when both the
font and XmString label are available. Clearing a label or an allocation
failure therefore yields the normal ring geometry without a null XmString
query.

The focused source contract is checked by
`tests/automated/check-progress-ring-geometry-label.py`.
