# TextField string getter guards

The public `XmTextFieldGetString`, `XmTextFieldGetStringWcs`, and
`XmTextFieldGetLastPosition` getters now handle a NULL widget before asking Xt
for an application context. They return the existing empty-result values
(`NULL` for allocated strings and `0` for the position) without dereferencing
invalid input; valid widget and conversion behavior is unchanged.

The focused source contract is checked by
`tests/automated/check-textfield-string-allocation.py`.
