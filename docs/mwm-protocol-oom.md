# Mwm protocol allocation failure handling

Mwm creates per-screen atom arrays for the WM query and WM selection
protocols. The allocation results are now checked before any per-screen
dereference. A failed WM query allocation skips only query ownership, while a
failed WM selection allocation returns from that setup helper after reporting
the error. Normal allocations and atom ownership are unchanged.

`tests/automated/check-mwm-protocol-oom.py` guards these failure paths.
