# DropDown multibyte-conversion cleanup

DropDown converts each selected `XmString` to multibyte text before inserting
it into the text field.  If `mbstowcs` rejects invalid locale input, the
conversion skips that item; the unparse buffer must still be released on this
error path.  The failure branch now frees the buffer before continuing,
preventing a leak while preserving the existing warning and skip behavior.

The regression contract is checked by
`tests/automated/check-dropdown-multibyte-failure-cleanup.py`.
