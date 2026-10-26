---
nav_exclude: true
---

# DataField raw primary-selection fallback

The raw wide-character primary-selection fallback now checks its conversion
buffer before calling `mbstowcs`. If allocation fails, the temporary
selection-move max-length override and callback state are restored/released
before returning, avoiding a null dereference.

The focused source contract is checked by
`tests/automated/check-datafield-primary-fallback.py`.
