# Text-source pending sentinel width

`_XmStringSourceSetPending` accepts either a Boolean snapshot pointer or the
legacy false/true sentinel values encoded as pointers.  The sentinel check now
uses `uintptr_t`, preserving the API behavior while avoiding a pointer-to-
`long` narrowing assumption on platforms with different integer widths.

The focused source contract is checked by
`tests/automated/check-text-source-pending-pointer-width.py`.
