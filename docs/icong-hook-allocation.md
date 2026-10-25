# IconG hook allocation handling

IconG's secondary-object, set-values, and get-values hooks now check every
extension-object and extension-data allocation.  Partial objects are released
before returning, and the get-values posthook tolerates a missing record when
its prehook could not establish temporary state.  Normal successful hook
behavior and cache ownership remain unchanged.

The regression check is `tests/automated/check-icong-hook-allocation.py`.
