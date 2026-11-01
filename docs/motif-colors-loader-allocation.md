---
nav_exclude: true
---
# Color selector allocation handling

The RGB color loader now keeps `ColorInfo` growth transactional and aborts
cleanly when a realloc or size calculation fails. Building the temporary
XmString list also checks its allocation before filling it. The existing color
selector state is left intact instead of dereferencing a null allocation.

Both allocator byte counts are checked against Xt's `Cardinal` size limit
before conversion. The loader also uses `size_t` for name lengths and caps
accepted entries at the signed-short range used by the legacy `num_colors`
field, so an oversized database cannot wrap the stored count or its sort/list
lengths.

The focused source regression is
`tests/automated/check-colors-loader-allocation.py`.
