---
nav_exclude: true
---

# Unicode mnemonic keysyms

Label and label-gadget mnemonics now recognize the X11 Unicode keysym form
(`0x01000000 | UCS`) and encode valid Unicode scalar values as UTF-8. Legacy
keysyms retain the historical low-byte behavior, while invalid scalar values
are rejected safely.

Regression check:

```text
python3 tests/automated/check-keysym-unicode-encoding.py
```
