---
nav_exclude: true
---
# Mwm screen-conflict warning bounds

During startup, `mwm` reports when another window manager already owns a
screen.  The localized message was formatted into an 80-byte stack buffer,
which could overflow for a long catalog translation.

The warning is now measured and allocated for its complete rendered length.  A
short fallback is emitted only if message allocation fails; screen-management
behavior is unchanged.

`tests/automated/check-mwm-screen-conflict-warning.py` protects this contract.
