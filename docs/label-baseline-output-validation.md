---
nav_exclude: true
---

# Label gadget baseline output validation

`XmLabelGadgetGetBaselines` now rejects a null widget or output pointer before
accessing gadget state. Valid baseline calculation and caching remain
unchanged, while malformed geometry queries no longer crash.

The focused regression check is
`tests/automated/check-label-baseline-outputs.py`.
