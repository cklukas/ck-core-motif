# BaseClass resource-copy allocation guards

When a composite widget class has an uncompiled resource list,
`BaseClass.c` copies the list into Xt-owned storage. The copy now checks both
native `size_t` multiplication and the Xt `Cardinal` limit before allocating,
so a representable host size cannot silently truncate on Xt’s allocation API.

The focused source-contract check is
`tests/automated/check-baseclass-resource-copy-allocation.py`.
