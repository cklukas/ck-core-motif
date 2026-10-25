# Make file-spec qualification failures deterministic

`_XmOSQualifyFileSpec` is a `void` helper that reports its results through two
output pointers.  Its allocation-failure branches incorrectly returned
`NULL`, which is invalid for a `void` function and fails with modern compiler
diagnostics.  The branches now return normally after releasing any partial
allocation.

Both output pointers are also cleared before allocation begins.  A caller
that receives an allocation failure therefore cannot accidentally consume an
uninitialized directory or pattern pointer, while successful qualification
continues to return the same strings as before.
