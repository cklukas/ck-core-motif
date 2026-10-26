# Icon-name hash tail selection

The icon-name cache hash now incorporates the final character of a non-empty
name. The previous implementation indexed the terminating NUL byte, causing
many names with the same first character and length to collide. Empty names
continue to use index zero safely.

The focused source regression is
`tests/automated/check-icon-name-hash-tail.py`.
