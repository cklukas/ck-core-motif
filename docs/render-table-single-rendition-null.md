# Null render-table lookup

`XmRenderTableGetRendition` now returns `NULL` when passed a null table. This
matches the failure behavior of the other render-table lookup helpers and
prevents the private display lookup from dereferencing an invalid handle.
Valid lookups and rendition ownership are unchanged.

Display-less tables are also supported: the lookup uses the process lock when
there is no display-backed application context.
