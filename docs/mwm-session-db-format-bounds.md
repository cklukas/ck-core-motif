# MWM session DB path formatting

When MWM derives its session client database path from the home directory, it
allocates room for the path and restart suffixes. The path is now written with
`snprintf` and its return value is checked; a formatting failure or unexpected
truncation releases the allocation and leaves the database name unset rather
than retaining a malformed path.

The focused source contract is checked with:

```sh
python3 tests/automated/check-mwm-session-resource-bounds.py
```
