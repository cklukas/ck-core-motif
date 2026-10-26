# Text selection-array byte counts

Text input resource updates and initialization now compute the guarded
selection-array byte count once and reuse it for both allocation and copying.
This keeps the `SIZE_MAX`/`UINT_MAX` checks tied to the exact size passed to
Xt and `memcpy`, avoids signed-count conversion warnings, and preserves the
existing built-in-array fallback when the requested count is not representable
or allocation fails.

The focused source contract is checked by
`tests/automated/check-text-selection-array-byte-count.py`.
