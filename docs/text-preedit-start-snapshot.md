---
nav_exclude: true
---
# XmText preedit-start snapshot handling

Overstrike input-method startup now validates the source scan range and checks
the saved text snapshot before activating the preedit.  If the range is
invalid or the snapshot cannot be obtained, the preedit state is cleared and
startup is rejected safely.

Regression coverage: `tests/automated/check-text-preedit-start-snapshot.py`.
