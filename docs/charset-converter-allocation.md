# Character-set table conversion allocation

The string-to-character-set-table converter uses a mutable copy of the input
and one combined result allocation. Both allocations are now checked. A failed
input copy returns `False`, and a failed result allocation releases the input
copy before returning, while normal parsing and result ownership remain
unchanged.
