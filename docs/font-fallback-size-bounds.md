# Font fallback name size bounds

The scaled-font fallback in `lib/Xm/FontS.c` constructs an XLFD by joining
two pieces of the requested font name with `-140`.  The resulting allocation
must fit both `size_t` arithmetic and Xt's `Cardinal` allocation size.  The
fallback now checks those bounds before allocating and formatting the name;
an unrepresentable name follows the existing unavailable-font error path
instead of wrapping an allocation size.

`tests/automated/check-font-fallback-size-bounds.py` keeps the checked size
calculation and matching `snprintf` capacity coupled in source review.
