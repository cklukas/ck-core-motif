# Drag target-table allocation handling

`ReadTargetsTable` grows the process-local cache after validating the X
property.  It now checks both initial table allocation and entry-array
reallocation before using the resulting pointers.  Allocation failure leaves
the property rejected instead of dereferencing a null table or losing the
existing entry-array pointer.
