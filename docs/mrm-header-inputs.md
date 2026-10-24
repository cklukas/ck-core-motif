# Mrm header helper inputs

Header loading, updating, RID access, and initialization now reject missing
file/output handles and incomplete header-record allocations before accessing
record fields.  Valid header operations retain their existing behavior.

The source-level regression is `tests/automated/check-mrm-header-inputs.py`.
