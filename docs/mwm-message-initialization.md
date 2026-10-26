# MWM message initialization

`initMesg()` now returns after its first completed initialization.  The
localized confirmation strings are process-global, so reinitializing them for
each screen or dialog only leaked the previous catalog allocations.  The
existing fallback strings and localized message selection remain unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-message-initialization.py
```
