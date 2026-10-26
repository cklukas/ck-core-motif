# Text selection empty fallback buffer

When a selection cannot be decoded into locale text, the transfer path creates
a one-byte empty fallback buffer.  That allocation is now checked before the
terminator is written; failure releases the incoming value and rejects the
follow-up operation cleanly.

The focused source contract is checked by
`tests/automated/check-text-selection-empty-buffer.py`.
