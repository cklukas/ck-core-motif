# UIL machine-list formatting bounds

The UIL machine-code listing uses 132-byte temporary lines while rendering
resource names, class names, tags, and decoded values.  Those values can be
longer than the line, and the old `sprintf`/`strcat` calls could overwrite
adjacent compiler state.

All machine-list formatting now goes through a size-aware `snprintf` helper;
the few incremental additions use a capacity-aware append helper.  Normal
lines retain their existing text, while oversized descriptions are truncated
at the listing line boundary without corrupting the compiler.

`tests/automated/check-uil-machine-list-bounds.py` protects the formatting
contract.
