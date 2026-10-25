# Clipboard external-selection output guards

`ClipboardGetSelection` now validates all output pointers before starting an
Xt selection request or event loop. Calls with missing value, type, size, or
format outputs fail immediately and leave the caller state untouched.
