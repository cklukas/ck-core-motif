# UIL section-link allocation failure handling

The value-binding and identifier-declaration semantic actions now check the
section-list node allocation before linking it.  If the compiler cannot
allocate that bookkeeping node, the action returns through its existing
error-safe path instead of dereferencing a null section entry.

The source-contract regression is
`tests/automated/check-uil-section-link-allocation.py`.
