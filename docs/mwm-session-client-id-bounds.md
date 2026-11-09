---
nav_exclude: true
---

# MWM session client-ID bounds

`ProcessSmClientID` reads a bounded X11 `SM_CLIENT_ID` property and copies it
into MWM-owned storage.  It now requires `bytes_after` to be zero before
accepting the value.  A property exceeding the bounded read is rejected rather
than persisting a prefix that could identify the wrong session client.

`tests/automated/check-mwm-session-client-id-bounds.py` protects this contract.
