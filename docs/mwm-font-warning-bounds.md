# Mwm font-load warning bounds

When a configured render table cannot provide a default font, `mwm` reports the
font list and exits.  The old path formatted that diagnostic into the shared
`wmGD.tmpBuffer` (`MAXBUF` bytes), so a long localized format could overwrite
storage before the fatal exit.

The diagnostic now allocates its measured rendered length and keeps the same
warning and exit behavior.  A short fallback is used if allocation fails.

`tests/automated/check-mwm-font-warning-bounds.py` protects this contract.
