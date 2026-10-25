# Icon visibility guards

`IconVisible` now returns `False` when client, screen, icon-box, clip-widget,
or scrolled-widget state is incomplete.  Initialized icon boxes continue to
use the existing geometry visibility checks.

`tests/automated/check-icon-visible-guards.py` protects the contract.
