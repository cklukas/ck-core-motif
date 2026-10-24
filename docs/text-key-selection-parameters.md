# Text key-selection parameters

The `key-select` action now initializes its cursor and selection endpoints to
the current insertion position before parsing an action parameter.  An
unknown direction therefore becomes a harmless no-op instead of allowing an
uninitialized position or range to reach selection scanning and cursor
updates.  Valid horizontal and vertical action parameters retain their
existing behavior.

`tests/automated/check-text-key-selection.c` exercises the invalid-parameter
case against the installed library.
