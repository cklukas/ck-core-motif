# DataField state accessor guards

DataField state, cursor, insertion, and selection APIs now reject NULL widgets
before Xt application-context lookup, preserving their established empty or
no-op results. `XmDataFieldGetSelectionPosition` also rejects NULL output
pointers before writing the selected range. Valid DataField editing,
conversion, selection, and callback behavior is unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-state-accessors.py`.
