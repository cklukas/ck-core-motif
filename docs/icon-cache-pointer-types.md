# Icon cache pointer conversions

The icon-directory cache stores related opaque directory handles in distinct
union and structure pointer types.  The cache boundary intentionally converts
between those representations, but implicit assignment is rejected by modern
compilers.  The conversions are now explicit at both ownership boundaries;
the cache layout and public icon APIs are unchanged.
