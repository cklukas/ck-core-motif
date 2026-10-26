# Legacy charset canonicalization bounds

The compatibility charset canonicalizer copies arbitrary charset names into an
owned string.  Its length now remains `size_t` and is checked against Xt's
allocation width before adding the terminator.  Oversized names fail cleanly
instead of truncating an `int` length.

The focused source contract is checked by
`tests/automated/check-legacy-charset-canonicalization.py`.
