# Mrm error-message allocation

Memory-mode `Urm__UT_Error` now allocates the replacement saved message before
freeing the previous one.  If the allocation fails, the error status still
returns without dereferencing a null pointer and the prior saved message stays
valid.

The source-level regression is
`tests/automated/check-mrm-error-allocation.py`.
