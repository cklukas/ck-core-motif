---
nav_exclude: true
---
# FontSelector portability hygiene

`XmFontSelector` keeps its supported resolution table in the same signed-short
representation used by `FontInfo`, so resolution selection cannot narrow an
arbitrary `int` into widget state. Iteration over Xt counts now uses `Cardinal`,
and the encoding-menu local index has an independent name rather than shadowing
the outer Xt argument index. `GetValuesHook` compares `XtArgVal` through its
documented integer representation before treating it as an output address.

These changes preserve the legacy public API while removing conversion,
signedness, shadowing, and pointer-versus-integer diagnostics under the strict
compiler profile. The focused source contract is covered by
`tests/automated/check-fontselector-portability.py`.
