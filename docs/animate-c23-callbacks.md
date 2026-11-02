# Animate demo C23 callbacks

The Animate demonstration keeps its existing Mrm callback registration and
animation behavior, but its application entry point, work procedure, and UI
callbacks use exact typed C declarations rather than legacy K&R definitions or
unspecified forward declarations.  This makes the demo build with the current
C23 configuration without changing its UID interface.

The focused source-contract check is:

```sh
python3 tests/automated/check-animate-c23-callbacks.py
```
