# Icon-list source guard

`ExtendIconList` now rejects an inconsistent icon-box state where existing
placement entries are reported but the source list is absent.  This prevents a
null source pointer from reaching the copy operation while leaving valid list
growth unchanged.

`tests/automated/check-icon-list-source.py` protects the validation contract.
