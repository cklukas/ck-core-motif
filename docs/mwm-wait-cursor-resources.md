# MWM wait-cursor bitmap resources

`ShowWaitState()` now checks both bitmap resources before creating the wait
cursor.  If either bitmap creation fails, any partial resource is freed and the
function returns without issuing an invalid `XCreatePixmapCursor()` call.
Successful wait-cursor creation and pointer/keyboard grabs are unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-wait-cursor-resources.py
```
