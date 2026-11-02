---
nav_exclude: true
---

# Label baseline output validation

`XmLabelGetBaselines` and `XmLabelGadgetGetBaselines` reject a null widget or
output pointer before accessing widget state. Valid baseline calculation and
caching remain unchanged, while malformed generic geometry queries no longer
crash one class while succeeding safely for the other. The Label path also
rejects an invalid negative line count before converting it to its internal
unsigned count.

The focused regression check is
`tests/automated/check-label-baseline-outputs.py`.
