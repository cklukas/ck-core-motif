---
nav_exclude: true
---
# Mwm display environment bounds

The window manager exports the active X display as `DISPLAY=...` for launched
clients.  The old path copied the X display name through two fixed 256-byte
arrays, so a long hostname, transport endpoint, or display suffix could
overwrite stack storage before the environment value was installed.

`InitWmDisplayEnv` now allocates the persistent environment string from the
complete display-name length and formats it with a bounded write.  The emitted
`DISPLAY=` value and `putenv` lifetime remain unchanged for ordinary displays.

`tests/automated/check-mwm-display-env-bounds.py` protects this contract.
