---
nav_exclude: true
---

# WSM null input streams

The primitive WSM decoders now return a safe zero value for a null byte stream,
and string decoding returns `NULL` without allocating. Empty or truncated
messages therefore fail through the existing higher-level validation paths
instead of crashing while reading the first byte.
