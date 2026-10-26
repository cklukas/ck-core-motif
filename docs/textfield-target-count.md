---
nav_exclude: true
---
# TextField target counts

TextField target negotiation now rejects callbacks without a transfer ID or
with target counts beyond the legacy processing limit before scanning external
atom data.

Regression coverage: `tests/automated/check-textfield-target-count.py`.
