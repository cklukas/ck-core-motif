---
nav_exclude: true
---
# Mrm creation callback record bounds

Mrm widget creation now validates both legacy and current serialized callback
descriptors before translating or fixing them up.  The descriptor header, the
signed callback count, and the count-plus-terminator item storage must fit
inside the widget record.  Valid callback records keep their existing callback
lookup and invocation behavior.

The source-level regression is
`tests/automated/check-mrm-callback-record-bounds.py`.
