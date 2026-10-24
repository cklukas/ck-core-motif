# Mrm literal diagnostic bounds

Mrm literal-fetch and color-literal error paths format into fixed 300-byte
buffers. Both paths now use `snprintf`, preserving normal diagnostics while
safely truncating long literal indexes and malformed input text.

The source-level regression is
`tests/automated/check-mrm-literal-error-bounds.py`.
