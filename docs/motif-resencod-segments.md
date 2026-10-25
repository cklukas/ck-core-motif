# Extended compound-text segment buffers

Both compound-text extended-segment decoders now check temporary charset and
text buffer allocations. A failed text allocation releases the charset copy
and returns a conversion error before either buffer is written.

The regression check is
`tests/automated/check-resencod-segment-allocation.py`.
