# MWM icon-list allocation

Icon boxes create and grow their placement list when icon cells are required.
Both paths now validate native `size_t` and Xt `Cardinal` allocation limits,
then compute allocation, copy, and initialization byte counts in `size_t`.
Large valid counts therefore cannot overflow signed intermediate arithmetic
before the request reaches the allocator.

The focused source contract is covered by
`tests/automated/check-mwm-icon-list-allocation.py`.
