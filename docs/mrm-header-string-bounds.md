# Mrm UID header string bounds

UID header fields have fixed-width on-disk storage. Header loading now requires
each field to contain a NUL terminator within its recorded width before copying
it into the in-memory `IDBFile`; malformed fields return `MrmNOT_VALID` instead
of allowing `strcpy` to read into adjacent header data or overflow a destination.
Valid headers retain their existing strings and version checks.

The source-level regression is
`tests/automated/check-mrm-header-string-bounds.py`.
