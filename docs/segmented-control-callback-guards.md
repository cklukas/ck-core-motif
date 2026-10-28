---
nav_exclude: true
---

# SegmentedControl callback guards

Toggle-child notifications and key handlers can be delivered during widget
teardown or synthetic dispatch. The control now treats missing closure,
child, or event data as a no-op; normal selection and keyboard navigation are
unchanged for live controls.

The focused contract check tolerates normal C line wrapping while requiring
both NULL-guard clauses: `tests/automated/check-segmented-control-callback-guards.py`.
