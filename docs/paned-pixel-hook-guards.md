---
nav_exclude: true
---

# Paned pixel hook guards

Paned's parent, child, and opposite-orientation pixel converters now validate
their widget/value inputs and, for child resources, the Paned parent class
before reading orientation state. Invalid exports return safely; invalid
imports return `XmSYNTHETIC_NONE` without changing widget state.

The focused source-contract check is:

```sh
python3 tests/automated/check-paned-pixel-hook-guards.py
```
