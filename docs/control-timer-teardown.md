---
nav_exclude: true
---

# Control timer and teardown guards

Several additive controls receive input or deferred delivery after an Xt
destruction sequence has begun.  Their public entry points now reject that
state without examining child data or scheduling more work.

- Badge and SegmentedControl return their established neutral failure values
  from affected public queries and mutators.
- Link, Toast, TeachingTip, and Spinner reject late event or timer delivery.
- A delayed TeachingTip also checks that its owner and popup shell are still
  alive before using them.
- Toast cancels and clears an old timeout identifier before starting a new
  display interval, and changing `XmNtoastTimeout` while shown restarts that
  interval using the replacement value.

These changes preserve normal control behavior while making repeated show,
pause, resource-update, and destruction sequences deterministic.
