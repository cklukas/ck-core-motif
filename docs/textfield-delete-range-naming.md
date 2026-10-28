# TextField delete-target range naming

The TextField conversion callback keeps a selection range for ordinary text
exports. Its delete-target branch now names its own primary-selection range
`delete_left` and `delete_right`, rather than redeclaring `left` and `right`.
This removes the compiler shadowing warning and makes it explicit that the
delete operation uses the primary range while preserving the public transfer
and deletion behavior.

The source contract is covered by
`tests/automated/check-textfield-delete-range-naming.py`.
