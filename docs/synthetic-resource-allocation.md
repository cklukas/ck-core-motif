# Synthetic-resource allocation

`_XmBuildResources` now validates signed superclass/subclass counts, source
list pointers, and the complete byte-size calculation before allocating the
combined synthetic-resource table.  Failed allocation leaves the caller’s
existing list unchanged instead of copying through a null pointer.

The focused source-level regression is
`tests/automated/check-synthetic-resource-allocation.py`.
