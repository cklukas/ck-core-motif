---
nav_exclude: true
---

# TextField XIM overstrike recovery width

TextField overstrike recovery derives its span from `XmTextPosition`, but the
legacy callback stored that span in `int` and used an `int` index while
assembling the restored text.  The recovery span now retains the position
width and the local index uses `size_t`, preventing truncation for large text
fields while preserving normal preedit behavior.

`tests/automated/check-textfield-preedit-recovery-width.py` protects this
contract.
