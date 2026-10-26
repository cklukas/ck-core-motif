---
nav_exclude: true
---

# TextField preedit recovery allocations

The wide-character overstrike recovery path now checks both its temporary
multibyte conversion buffer and its recovered wide-character buffer before
writing into them.  Failed recovery releases the current wide buffer and
returns without continuing the preedit update.

`tests/automated/check-textfield-preedit-recovery-allocations.py` protects these
guards.
