# Legacy file-selection restore bounds

The compatibility file-selection restore action concatenates the directory and
pattern strings into the filter field.  Lengths now remain `size_t` and the
combined size is checked before the allocation is narrowed to Xt's type.
Unrepresentable values are rejected with the existing cleanup path.

The focused source contract is checked by
`tests/automated/check-legacy-filesel-restore-bounds.py`.
