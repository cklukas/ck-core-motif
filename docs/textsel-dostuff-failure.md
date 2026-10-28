---
nav_exclude: true
---
# Text malformed transfer completion

`DoStuff` now explicitly fails a transfer when callback state is incomplete,
provided a transfer ID is available. Teardown and cancellation paths therefore
cannot leave an external selection request pending indefinitely.

Regression coverage: `tests/automated/check-textsel-dostuff-failure.py`.
