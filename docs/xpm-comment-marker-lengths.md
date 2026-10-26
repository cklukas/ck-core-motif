# XPM comment marker length checks

The in-memory XPM comment parser uses the configured beginning and ending
comment markers with `strncmp` and pointer advances. Their lengths now remain
`size_t`, matching `strlen` and the C library interfaces, instead of narrowing
before those operations. The loop index used to copy the ending marker uses the
same width. This preserves behavior for ordinary XPM data while avoiding
truncation for unusually large marker strings.

`tests/automated/check-xpm-comment-marker-lengths.py` keeps the source contract
under regression coverage.
