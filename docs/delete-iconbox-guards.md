---
nav_exclude: true
---
# Icon-box deletion guards

`DeleteIconFromBox` now rejects incomplete icon-box/client geometry before Xt
queries or division, validates WSM workspace metadata, and skips teardown calls
when the child widget is already absent.  Valid deletion remains unchanged.

`tests/automated/check-delete-iconbox-guards.py` protects the contract.
