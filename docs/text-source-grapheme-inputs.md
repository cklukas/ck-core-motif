---
nav_exclude: true
---
# Text-source grapheme-step inputs

The Unicode grapheme-step helper now validates its source callback, private
data, and attached widget before dispatching or building a Unicode map. Missing
or partially torn-down state preserves the requested position instead of
dereferencing invalid storage. Custom source callbacks retain their existing
dispatch behavior.

The focused source contract is checked by
`tests/automated/check-text-source-grapheme-inputs.py`.
