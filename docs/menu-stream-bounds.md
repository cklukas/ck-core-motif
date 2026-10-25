# Mwm menu command stream bounds

`ParseClientCommand` joins a command fragment and its continuation with one
space before parsing. Its allocation now reserves both that separator and the
terminating NUL, and rejects arithmetic that cannot fit in the Xt allocation
type. Existing menu syntax and parsing behavior are unchanged for
representable input.

`tests/automated/check-menu-stream-bounds.py` guards the allocation contract.
