---
nav_exclude: true
---

# Unicode mnemonic keysyms

Label and label-gadget mnemonics now recognize the X11 Unicode keysym form
(`0x01000000 | UCS`) and encode valid Unicode scalar values as UTF-8. Legacy
keysyms retain the historical low-byte behavior, while invalid scalar values
are rejected safely.

The Label and LabelG drawing paths reserve five bytes for this conversion:
four for the largest UTF-8 scalar and one terminating NUL.  They therefore do
not depend on a platform's `MB_LEN_MAX` when a non-BMP mnemonic is rendered.

Regression check:

```text
python3 tests/automated/check-keysym-unicode-encoding.py
```
