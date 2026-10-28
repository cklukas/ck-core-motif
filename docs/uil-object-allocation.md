# UIL object and section allocation failure handling

The UIL object, child-object, and section-link semantic actions now check
their symbol-node allocations before initialization or linking.  Object
creation selects the existing null-frame path when allocation is unavailable;
section linking simply returns without dereferencing a missing bookkeeping
node.  This keeps parser state valid under resource pressure and avoids
partial symbol-table writes.

The source-contract regression is
`tests/automated/check-uil-object-allocation.py`.
