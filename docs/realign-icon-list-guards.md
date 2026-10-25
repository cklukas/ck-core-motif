# Icon-list realignment guards

`RealignIconList` now rejects missing placement storage, non-positive old or
new dimensions, invalid cell sizes, and incomplete root coordinate arrays
before converting or moving entries.  Valid icon-box realignment is unchanged.

`tests/automated/check-realign-icon-list-guards.py` protects the contract.
