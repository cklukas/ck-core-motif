---
nav_exclude: true
---

# DataField picture callback input

`PictureVerifyCallback` now validates its widget and callback data before
reading the text verification record. A missing widget or callback record is
ignored, while a record without its text edit payload is rejected by clearing
`doit`. This keeps malformed embedder callbacks from dereferencing null input
and leaves normal picture verification unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-picture-callback-input.py`.
