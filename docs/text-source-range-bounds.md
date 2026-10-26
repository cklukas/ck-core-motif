# Text source range bounds

Legacy text-source reads now cap position advances before `position +
available` can overflow an integer-backed path. `GetString` also rejects an
incomplete source read rather than returning a silently truncated string, for
both narrow and wide results.

The focused source contract is checked by
`tests/automated/check-text-source-range-bounds.py`.
