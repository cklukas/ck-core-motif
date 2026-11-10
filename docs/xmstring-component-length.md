# XmString component lengths

`XmeStringGetComponent` exposes rendition and tag lengths as `unsigned int`
while native string lengths are `size_t`.  The component path now checks the
conversion before narrowing and ends the context safely if a length cannot be
represented.  Copied component data is released on that failure path.

The extractor also keeps text segment byte counts in the same unsigned type as
the Compound String representation and public output. A valid count above
`INT_MAX` therefore cannot become negative while choosing a direction,
allocating a copied component, or returning its length.

The focused check is
`tests/automated/check-xmstring-component-length.py`.
