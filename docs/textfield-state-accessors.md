# TextField state accessor guards

TextField state getters and setters now reject a NULL widget before Xt
application-context lookup, returning their established empty values or doing
nothing. `XmTextFieldGetSelectionPosition` also rejects NULL output pointers
before writing the selected range. Valid state reads, updates, selection
behavior, and callback/resource semantics are unchanged.

The focused source contract is checked by
`tests/automated/check-textfield-state-accessors.py`.
