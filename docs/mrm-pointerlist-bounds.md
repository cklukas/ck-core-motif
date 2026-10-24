# Mrm pointer-list bounds

Mrm pointer lists now reject non-positive capacities and capacity products that
cannot be represented by `size_t` before allocating their pointer vectors.
Appending to a malformed or already-full list also refuses to double a slot
count past `INT_MAX`, avoiding signed overflow and wrapped allocator sizes.
Valid positive capacities and the existing append/resize behavior are unchanged.

The source-level regression is `tests/automated/check-mrm-pointerlist-bounds.py`.
