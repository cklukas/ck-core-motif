# Earth demo C23 callbacks and timer lifecycle

The Earth demonstration now uses typed C23 declarations for its application
entry point, timeout procedure, and Xt callbacks.  Its rotation timer is also
explicitly inactive at zero speed: startup does not divide by zero to create a
timeout, and changing the scale from zero to a nonzero value records the newly
created timeout ID.  Existing resource and callback behavior is otherwise
unchanged.

The focused source-contract check is:

```sh
python3 tests/automated/check-earth-c23-timer.py
```
