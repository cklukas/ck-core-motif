# Mwm screen-warp warning bounds

The `f.screen` command reports an attempt to warp to an unmanaged screen.  Its
message was formatted into an 80-byte stack buffer, which was unnecessary and
could overflow if the diagnostic were localized or extended.

The warning now allocates the exact rendered message length and keeps the bell
and command-failure behavior unchanged.  A short fallback is used only when
message allocation fails.

`tests/automated/check-mwm-screen-warp-warning.py` protects this contract.
