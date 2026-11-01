# MWM timestamp property event

`GetTimestamp()` asks X11 to generate a `PropertyNotify` event by appending
zero property items to MWM's own `WM_STATE`.  With zero items, Xlib does not
read a property payload. The call now supplies `NULL` rather than the address
of an uninitialized local, making that zero-length contract explicit while
preserving the server round-trip and event timestamp behavior.

Run the focused check with:

```
python3 tests/automated/check-mwm-timestamp-property-event.py
```
