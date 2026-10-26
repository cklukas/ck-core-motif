# MWM screen client-list bounds

`UpdateScreenClientList` now checks the next element count and byte product
before reallocating its window array. It also converts the final count
explicitly to the unsigned type used by `SetMwmClientList`, preserving the
existing property update behavior while avoiding size and conversion hazards.

The source contract is covered by
`tests/automated/check-screen-client-list-guards.py`.
