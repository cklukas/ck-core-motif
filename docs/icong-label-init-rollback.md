---
nav_exclude: true
---
# IconGadget label initialization rollback

IconGadget now checks the private copy of an initial `XmNlabelString`.  If the
copy fails, it rebuilds the label from the gadget name and guards geometry
measurement when no fallback can be allocated, preventing NULL compound-string
use during construction.

The focused regression is
`tests/automated/check-icong-label-init-rollback.py`.
