# Icon-box keypress callback guards

`HandleIconBoxIconKeyPress` now returns when its icon widget or key event is
missing, before reading the widget window or writing event fields.  Valid key
handling remains unchanged.

`tests/automated/check-keypress-callback-guards.py` protects the contract.
