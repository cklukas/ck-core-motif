---
nav_exclude: true
---
# Warning output safety

Motif warning handlers receive text from the error database and substitution
parameters supplied by widgets. The handler now measures the formatted header
and message before allocating them, then allocates enough space for indentation
after embedded newlines. Long diagnostics remain readable without overwriting
the neighboring warning state.

`tests/automated/check-warning-buffer.py` checks the source contract, and the
native ARM64 library build verifies the implementation compiles cleanly.
