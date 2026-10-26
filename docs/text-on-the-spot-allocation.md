# Text on-the-spot state allocation

Text initialization allocates a small record used by on-the-spot input
composition.  The record is now checked before its fields are initialized;
allocation failure leaves it null for the existing guarded composition and
teardown paths instead of dereferencing null storage.

The focused source contract is checked by
`tests/automated/check-text-on-the-spot-allocation.py`.
