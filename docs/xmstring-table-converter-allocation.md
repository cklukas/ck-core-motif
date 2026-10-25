# XmString-table conversion allocation

The XmString-table resource converter now checks each per-item string
allocation, initial table allocation, growth reallocation, and generated
XmString. Any failure releases the partial table and strings before returning
`False`; successful conversion and normal destroy ownership remain unchanged.
