# TextField baseline callback bounds

`TextFieldGetBaselines` now validates its widget and output pointers, reports
allocation failure without publishing partial state, and computes the baseline
metric in widened arithmetic before clamping it to `Dimension`. Normal
one-baseline callbacks retain their existing result and ownership contract.

The focused source contract is checked by
`tests/automated/check-textfield-baselines.py`.
