# Text right-to-left translation allocation

Right-to-left Text initialization copies an alternate event translation
string before parsing it.  The copy now checks the length and Xt allocation
limit and skips only that optional override if storage is unavailable.  The
widget continues through normal initialization without dereferencing a null
buffer.

The focused source contract is checked by
`tests/automated/check-text-rtl-translation-allocation.py`.
