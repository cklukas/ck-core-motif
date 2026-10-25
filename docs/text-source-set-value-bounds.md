---
nav_exclude: true
---
# Text source set-value bounds

`_XmStringSourceSetValue` accepts a C string and stores its length in the
legacy `XmTextBlock` integer field.  It now treats a null value as an empty
string and rejects a string whose length cannot be represented by that field,
without changing selection or source state.  Representable values continue
through the existing replacement and callback path.

`tests/automated/check-text-source-set-value-bounds.py` protects the checked
conversion and null-input behavior.
