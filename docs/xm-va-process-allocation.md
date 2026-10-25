Simple-menu varargs allocation failures
========================================

The shared simple-menu varargs processor now validates its normalized `Arg`
array size and button metadata counts before allocation.  It checks every
temporary allocation, frees partial results on failure, and returns failure to
the six public simple-menu constructors, which now unlock and return `NULL`
without dereferencing incomplete arrays.

Successful menu construction and public function signatures are unchanged.
