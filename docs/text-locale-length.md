---
nav_exclude: true
---
# Locale-text input length

Selection text lengths arrive as `unsigned long`, but the legacy Motif text
conversion path and its allocation contract are `INT_MAX`-bounded.  The
converter now rejects larger lengths before calling
`XmbTextPropertyToTextList`, preventing an oversized remote selection from
driving an unbounded Xlib decode.

The focused regression is `tests/automated/check-text-locale-length.py`.
