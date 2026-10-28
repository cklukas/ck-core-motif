---
nav_exclude: true
---
# TextField target counts

TextField target negotiation now rejects callbacks without a transfer ID or
with target counts beyond the legacy processing limit before scanning external
atom data. The companion target-list scan uses the callback's unsigned count
type for its index as well, avoiding a signed comparison when an external
owner supplies the target list.

Regression coverage: `tests/automated/check-textfield-target-count.py`.
