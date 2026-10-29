# DataField mutator input guards

The public DataField string, replacement, and insertion helpers now return
before Xt application-context lookup when passed a NULL widget. The last
position getter follows the same rule and returns zero. Valid DataField text
conversion, editing, callbacks, and selection behavior are unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-mutator-inputs.py`.
