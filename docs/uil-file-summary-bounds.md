---
nav_exclude: true
---
# UIL file-summary bounds

The listing file summary prints every expanded source path into a 132-byte
line buffer.  Formatting the path directly with `snprintf` was memory-safe but
still produced a compiler truncation warning and left the available capacity
implicit.

The summary now measures the prefix, computes the remaining path capacity, and
copies only that many bytes before adding the terminator.  The existing
one-line summary format remains unchanged for normal paths, while unusually
long paths are visibly truncated at the line boundary without a diagnostic or
buffer overrun.

`tests/automated/check-uil-file-summary-bounds.py` protects this contract.
