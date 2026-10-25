# Text source forward-progress validation

Text source reads can fail by returning the current position without a block.
String extraction now clears the block length before each read and requires a
strictly advancing position in both byte and wide-character paths. A failed
or non-progressing source read therefore returns the partial result safely
instead of reusing stale block data or looping forever.

The regression check is
`tests/automated/check-text-source-progress.py`.
