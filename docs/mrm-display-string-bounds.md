# Mrm display-string conversion bounds

Font and font-set conversion prefixes a display pointer to a font name before
using the result as a cache key. The helper now rejects null or invalid size
inputs, checks the terminator-inclusive source length and allocation addition,
and copies the suffix directly. Valid calls retain the same cache-key bytes and
allocation sizing while malformed inputs return `NULL` safely.

The source-level regression is
`tests/automated/check-mrm-display-string-bounds.py`.
