---
nav_exclude: true
---

# Public mutator teardown guards

Public state-changing helpers must be safe when applications receive a late
event or callback while Xt is destroying the target widget.  Disclosure
expansion, RowActions reveal, TeachingTip hide, and Flyout hide now reject
that state before changing child layout, popup visibility, timers, or callback
delivery.  Query APIs retain their established, conservative teardown values.

`tests/automated/check-public-mutator-destroy-guards.py` keeps this contract
explicit for the affected entry points.
