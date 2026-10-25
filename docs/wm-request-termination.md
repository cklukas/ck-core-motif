# Mwm window-management request termination

Mwm reads `_DT_WM_REQUEST` as a sequence of NUL-separated byte strings. The
request extractor now treats an unterminated final property as a valid final
request, copies only the bytes returned by X, and appends its own terminator.
It no longer indexes one byte beyond the property or relies on `strncpy` to
read a terminator that may not exist.

`tests/automated/check-wm-request-termination.py` guards this behavior.
