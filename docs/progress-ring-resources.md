---
nav_exclude: true
---
# ProgressRing resource failure handling

ProgressRing now keeps its previous label when a replacement XmString cannot
be copied, and uses the track GC for text whenever the sensitive text GC is
unavailable. A partial resource failure therefore preserves visible text
instead of freeing the old label or issuing a draw with a null GC.

The focused source contract is checked by
`tests/automated/check-progress-ring-resources.py`.
