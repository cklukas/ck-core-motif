# Icon insertion geometry guards

`InsertIconInfo` now rejects incomplete icon-box geometry and missing widgets
before placement insertion.  This prevents divide-by-zero and invalid Xt
accesses when initialization has not produced usable increment or widget state.

`tests/automated/check-insert-icon-geometry-guards.py` protects the contract.
