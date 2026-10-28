# Standard conversion gadget parents

Scalar standard conversions now fail safely when a gadget has no parent from
which to obtain foreground, background, or colormap resources.  The converter
returns its already-initialized empty result instead of passing `NULL` to
`XtGetValues`.

Regression coverage: `tests/automated/check-standard-convert-gadget-parent.py`.
