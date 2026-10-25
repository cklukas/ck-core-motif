# Icon-info move guards

`MoveIconInfo` now treats missing placement storage, invalid source or
destination indices, self-moves, and empty source entries as no-ops.  This
prevents malformed workspace state from indexing outside the placement array
or dereferencing a missing client while leaving valid icon moves unchanged.
When workspace metadata is enabled, a missing per-workspace client record is
also tolerated while the placement list is updated.

`tests/automated/check-move-icon-info-guards.py` protects the validation
contract.
