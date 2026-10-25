---
nav_exclude: true
---
# Usable text concatenation bounds

Converting a compound string to locale or string text joins each valid segment
into one buffer.  The running byte count now uses `size_t` and checks the next
segment before adding it.  If the total cannot be represented, the conversion
frees the current segment and accumulated buffer, releases its context, and
returns `XNoMemory` instead of wrapping an `int` and under-allocating.
