# DataField baseline getter guard

`XmDataFieldGetBaseline` now returns zero for a NULL widget before Xt
application-context lookup. Valid baseline calculation and layout metrics are
unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-baseline-input.py`.
