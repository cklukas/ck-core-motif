# XmString separator allocation

`XmStringSeparatorCreate` now checks its cached entry table and each of the
two optimized entries.  A partial cache is released and `NULL` returned on
allocation failure, rather than initializing or cloning through null storage.
