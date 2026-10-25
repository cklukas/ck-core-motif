# Minimized-icon state placement guards

`ShowIconForMinimizedClient` now rejects missing client/workspace state and
invalid stored placement indices before reading the workspace placement list.
Valid minimized-icon placement continues unchanged.

`tests/automated/check-show-icon-state-place-guards.py` protects the contract.
