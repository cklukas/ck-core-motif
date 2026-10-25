# Client icon-state guard

`ShowClientIconState` now returns for a missing client record before reading
the icon window handle.  Existing state transitions for valid clients are
unchanged.

`tests/automated/check-show-client-icon-state.py` protects the contract.
