---
nav_exclude: true
---
# Toast SetValues transactions

`XmToast` now copies replacement message and action strings before releasing
the old values. Allocation failure preserves the previous content and avoids
publishing invalid child labels; layout negotiation only treats successful
replacements as changes.

The focused source check is
`tests/automated/check-toast-setvalues-redraw.py`.
