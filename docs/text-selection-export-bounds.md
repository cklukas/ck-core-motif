# Text selection export bounds

XA_STRING, COMPOUND_TEXT, and UTF-8 selection exports now check that the Xlib
property length fits the allocator’s `size_t` and allocate one byte for an
empty value. Copies are skipped when the length is zero, avoiding null-source
`memcpy` calls while preserving the reported protocol length.

The source contract is covered by
`tests/automated/check-text-selection-export.py`.
