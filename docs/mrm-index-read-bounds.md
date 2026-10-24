# Mrm read-side index bounds

Read-side index search and resource enumeration now use `MrmCount` for entry
indices, handle empty records without unsigned underflow, validate string
offsets and node metadata, and propagate buffer reload failures.  Malformed
child links and unterminated index strings now return a bad-record result
instead of crashing or walking arbitrary memory.

The source-level regression is
`tests/automated/check-mrm-index-read-bounds.py`.
