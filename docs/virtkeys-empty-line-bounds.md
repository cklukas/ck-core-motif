# Virtual-key empty-line bounds

`_XmVirtKeysLoadFileBindings` reads resource files in bounded chunks.  A
blank chunk is valid input (for example, an empty line between comments), so
the parser must not inspect `line[length - 1]` until it has established that
the length is nonzero.  The comment-continuation and comment-line paths now
share the measured length and safely handle empty chunks while preserving the
existing binding grammar.

The regression contract is checked by
`tests/automated/check-virtkeys-empty-line-bounds.py`.
