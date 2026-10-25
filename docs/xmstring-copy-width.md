---
nav_exclude: true
---
# Copied wide XmString segment width

The internal segment reader can copy component data for callers that request
owned text.  Wide-character components store `wchar_t` elements, while the
reported count is a character count.  The old copy path treated that count as
a byte count, under-copying wide text and leaving an incorrectly sized buffer.

The copy now multiplies wide counts by `sizeof(wchar_t)` before allocation and
copying, while retaining the existing byte-count behavior for ordinary and
multibyte components.  The source regression in
`tests/automated/check-xmstring-copy-width.py` protects the type-aware copy.
