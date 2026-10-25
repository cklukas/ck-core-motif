# TabList attribute cleanup

`XmTabAttributesFree` now accepts a `NULL` attribute record and returns without
dereferencing it.  This makes cleanup paths safe when an optional query did not
produce attributes.

The focused regression is `tests/automated/check-tablist-attributes-free.py`.
