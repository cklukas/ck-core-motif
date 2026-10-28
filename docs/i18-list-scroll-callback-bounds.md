# I18List scrollbar callback bounds

Scrollbar callback values are supplied as integers but the historical I18List
scroll helpers accept `short` amounts.  The callbacks now clamp values to the
representable `short` range before calling those helpers.  This prevents a
large externally supplied scrollbar value from wrapping into an opposite
scroll direction while preserving the existing internal and public API
contracts.

The source-contract regression is
`tests/automated/check-i18-list-scroll-callback-bounds.py`.
