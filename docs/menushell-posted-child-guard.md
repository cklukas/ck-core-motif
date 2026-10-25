---
nav_exclude: true
---
# Posted MenuShell child guard

Recursive menu popdown can find a posted shell whose child has already been
removed. `PopdownKids` must check that nested shell before reading its first
child, just as it checks the current shell.

The nested empty-shell case now returns safely and leaves the existing normal
popdown recursion unchanged. The regression
`tests/automated/check-menushell-posted-child-guard.py` covers this teardown
boundary.
