---
nav_exclude: true
---
# Mrm header helper inputs

Header loading, updating, RID access, and initialization now reject missing
file/output handles and incomplete header-record allocations before accessing
record fields.  RID retrieval clears a valid caller output handle before it
checks the file input, so every rejected request leaves neutral output state.
Valid header operations retain their existing behavior.

The source-level regression is `tests/automated/check-mrm-header-inputs.py`.
