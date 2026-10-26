# MWM invoke-command parsing

`F_InvokeCommand` receives two 32-bit protocol values and an X atom from a
textual function argument.  It now scans them into matching unsigned
intermediate types before conversion, rather than writing through incompatible
`int *` casts or using a potentially mismatched `%ld` conversion.

The existing client-context dispatch behavior is unchanged.
