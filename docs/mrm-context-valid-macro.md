# Mrm resource-context validation macro

`UrmRCValid` now checks for `NULL` before reading the context validation field.
All existing valid-context checks keep the same result, while callers that pass
no context receive their normal bad-context path instead of a null pointer
dereference.
