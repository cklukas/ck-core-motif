---
nav_exclude: true
---

# Clipboard lock record validation

The clipboard lock property is serialized data and may be missing, truncated,
or otherwise invalid after an interrupted or incompatible client operation.
`ClipboardLock` now checks the lookup result and record size before reading the
lock fields. `ClipboardUnlock` applies the same checks and rejects non-positive
lock levels. Invalid records fail safely after releasing any allocated record;
the existing behavior for a missing property and valid lock records is
unchanged.

The focused regression check is
`tests/automated/check-clipboard-lock-record-validation.py`.
