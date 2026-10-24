# Mrm zero-count descriptor inputs

Mrm module and widget-record argument-list builders reject zero-count inputs
before computing their one-entry-minus-one descriptor sizes. This prevents
unsigned underflow and oversized context growth while leaving valid nonempty
UID records unchanged.

The source-level regression is `tests/automated/check-mrm-zero-counts.py`.
