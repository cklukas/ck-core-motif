---
nav_exclude: true
---
# Mwm configuration-reopen warning bounds

When an included resource file is popped from the configuration stack,
`mwm` reopens the previous file and reports failures with its pathname.  The
legacy diagnostic used a fixed `MAXWMPATH+1` buffer, so a long path or
localized format could overflow it.

The warning now allocates the complete rendered message after checking the
formatter result against Xt's `Cardinal` width and preserves the include-stack
recovery behavior. A short fallback is emitted if message allocation fails.

`tests/automated/check-mwm-config-reopen-warning.py` protects this contract.
