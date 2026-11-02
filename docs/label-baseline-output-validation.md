---
nav_exclude: true
---

# Label geometry-trait output validation

`XmLabelGetBaselines` and `XmLabelGadgetGetBaselines` reject a null widget or
output pointer before accessing widget state. Valid baseline calculation and
caching remain unchanged, while malformed generic geometry queries no longer
crash one class while succeeding safely for the other. The Label path also
rejects an invalid negative line count before converting it to its internal
unsigned count.

The matching display-rectangle traits now apply the same null widget and
`XRectangle` output validation, so generic geometry clients get consistent
safe failure from Label and LabelGadget.

The focused regression check is
`tests/automated/check-label-baseline-outputs.py`.
