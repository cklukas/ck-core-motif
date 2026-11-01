---
nav_exclude: true
---
# FontSelector font-list count bounds

`XmFontSelector` stores family and non-XLFD font counts in signed-short
fields, even though an X server can report many more names. The loader now
caps each retained list at the representable range, stops adding unrepresentable
families, and performs its allocation byte conversions only after checking Xt's
`Cardinal` limit. Sorting and assignments use size-correct, explicitly bounded
counts.

The focused source contract is covered by
`tests/automated/check-fontselector-font-count-bounds.py`.
