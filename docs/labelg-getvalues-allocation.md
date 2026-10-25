# LabelG get-values allocation handling

`GetValuesPrehook` creates a temporary cache extension object and pushes an
extension-data record for the matching posthook.  The prehook now checks both
allocations and releases a partial object on failure.  The posthook also
accepts an absent record, so an allocation failure cannot turn into a null
 dereference while unwinding the get-values operation.

The regression check is `tests/automated/check-labelg-getvalues-allocation.py`.
