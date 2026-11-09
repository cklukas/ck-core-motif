---
nav_exclude: true
---
# UIL machine-list formatting bounds

The UIL machine-code listing uses fixed-size temporary lines while rendering
resource names, class names, tags, and decoded values.  Those values can be
longer than the line, and the old `sprintf`/`strcat` calls could overwrite
adjacent compiler state.

All machine-list formatting now goes through a size-aware `snprintf` helper;
the few incremental additions use a capacity-aware append helper.  Normal
lines retain their existing text, while oversized descriptions are truncated
at the listing line boundary without corrupting the compiler.

The machine-code writer has a separate fixed line buffer.  Its text field
starts at the one-based `TEXT_COL`, so its terminating NUL is placed at
`TEXT_COL - 1 + text_len`.  This keeps a maximum-width resource description
inside the final valid byte rather than writing one byte past the buffer.

`tests/automated/check-uil-machine-list-bounds.py` protects the formatting
contract, and `tests/automated/check-uil-machine-text-terminator.py` protects
the machine-code field terminator.
