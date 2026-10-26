# MWM workspace-hints inputs

`GetWorkspaceHints` now rejects missing output pointers with `BadValue` before
dereferencing them. It also treats a successful workspace-hints provider call
that returns no record as `BadAlloc`, rather than continuing with a null
record. A provider result that advertises workspace atoms but supplies no atom
array is rejected with `BadValue` before the copy. Valid callers and successful
populated hints retain the existing behavior.

The focused source contract is covered by
`tests/automated/check-mwm-workspace-hints-inputs.py`.
