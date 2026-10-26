# TextField substring range validation

`XmTextFieldGetSubstring` and `XmTextFieldGetSubstringWcs` now validate the
output buffer, start position, requested length, and capacity before pointer
arithmetic or conversion. Requests beyond the current value retain the normal
truncated result; invalid requests fail without touching caller storage. A
valid zero-length wide request still writes its terminating null as before.

The focused source contract is checked by
`tests/automated/check-textfield-substring-ranges.py`.
