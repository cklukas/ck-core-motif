# Mwm icon-placement allocation bounds

Mwm’s icon-placement grid allocates row, column, and slot arrays from display
and icon dimensions. The allocation sizes now use `size_t` arithmetic, reject
values that cannot fit the Xt `Cardinal` allocator or the existing integer slot
count, and clean up earlier arrays on failure. Normal grid sizing and placement
order remain unchanged.

`tests/automated/check-icon-placement-allocation-bounds.py` guards these size
checks.
