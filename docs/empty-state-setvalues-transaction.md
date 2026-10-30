---
nav_exclude: true
---

# EmptyState text updates

EmptyState now copies replacement message and action strings before releasing
the current values. If either allocation fails, the existing visible string
remains owned and in use, and the widget avoids reporting a successful text
change for that resource.

The focused source contract is checked by
`tests/automated/check-empty-state-setvalues-transaction.py`.
