# Text secondary-resource allocation

The Text widget's secondary-resource hook builds a two-entry table for its
input and output subobjects.  The table allocation is now checked before the
entries are initialized.  The output pointer is cleared first, and allocation
failure returns zero resources without dereferencing null storage.

The focused source contract is checked by
`tests/automated/check-text-secondary-resource-allocation.py`.
