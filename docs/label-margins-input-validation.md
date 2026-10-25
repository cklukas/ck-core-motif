---
nav_exclude: true
---

# Label gadget margins input validation

`XmLabelGadgetMarginsProc` now treats a missing widget or margins record as a
safe no-op. Valid baseline-margin reads and writes retain their existing
behavior.

The focused regression check is
`tests/automated/check-label-margins-inputs.py`.
