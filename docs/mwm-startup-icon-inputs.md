# MWM startup and named-icon input boundaries

MWM derives its resource identity from `argv[0]` before creating its Xt
application state. The program entry point now rejects an empty or incomplete
argument vector before that access. `CopyArgv`, which is also used as a helper
by restart support, independently converts a negative count or a missing
non-empty vector to an empty restart state rather than dereferencing it or
retaining a negative secondary-display count.

Named icon lookup now also requires both an icon name and a valid X `Screen`.
This preserves the existing bitmap-directory and `XBMLANGPATH` lookup order
for valid calls while avoiding a null screen reaching Motif's pixmap APIs.

Focused check:

```
python3 tests/automated/check-mwm-startup-icon-inputs.py
```
