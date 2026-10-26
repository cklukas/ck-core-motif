---
nav_exclude: true
---
# XmText preedit verification cleanup

When a verify-preedit callback ends an overstrike composition early, `XmText`
now releases any temporary replacement buffer and the saved overstrike text,
then clears the preedit positions and state before returning.

Regression coverage: `tests/automated/check-text-preedit-verify-cleanup.py`.
