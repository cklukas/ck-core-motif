# BaseClass extension allocation handling

BaseClass lazily allocates class-extension records, secondary-resource data,
resource indirection tables, and widget-extension association nodes.  These
allocations are now checked before initialization or dereference.  Partial
secondary-resource records are released, and resource transformation reports
an empty result when an allocation fails.  Existing successful extension
setup and context ownership are unchanged.

The regression check is `tests/automated/check-baseclass-extension-allocation.py`.
