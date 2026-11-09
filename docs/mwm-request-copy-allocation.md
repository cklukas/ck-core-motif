---
nav_exclude: true
---

# MWM request-copy allocation

`mwm` copies each received desktop request before using the shared parser
buffer. If that copy cannot be allocated, it now reports the failure, releases
the received request, and skips only that request. It does not pass a null
string to the parser or disturb subsequent requests.

```sh
python3 tests/automated/check-mwm-request-copy-allocation.py
```
