# TabBox image-rotation size bounds

TabBox rotates XImages used by tab visuals.  The old helper accumulated the
required storage in signed `int` and passed the result directly to Xt, so
large or malformed image dimensions could wrap into an undersized allocation.
The helper now validates positive dimensions, computes packed and rotated
sizes in `size_t`, and rejects values that cannot fit Xt's `Cardinal` size.

`tests/automated/check-tabbox-image-size-bounds.py` keeps the checked
allocation path in place.
