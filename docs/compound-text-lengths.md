---
nav_exclude: true
---
# Compound-text segment lengths

Compound-text decoding now checks the combined encoding-prefix and segment
length before allocating or passing the size to Xlib. An unrepresentable
length is rejected cleanly; ordinary `XmRString` to `XmRXmString` conversion
remains unchanged.

The compound-text encoding registry export also checks the aggregate size of
all tag and encoding pairs before allocating the NUL-separated target. An
unrepresentable registry returns no target data instead of wrapping its length
or allocation size; see `tests/automated/check-encoding-registry-bounds.py`.
