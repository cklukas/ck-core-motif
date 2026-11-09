# Color object type boundaries

`XmColorObj` obtains its screen count from Xlib as a signed `int`, while Xt's
allocation API accepts a `Cardinal`.  Initialization allocates its per-screen
arrays only for a positive count and makes that signed-to-unsigned conversion
explicit at the allocation boundary.  An anomalous non-positive count follows
the established allocation-failure path rather than becoming a very large
allocation request.

The selection-timeout sentinel is represented as `(unsigned long)-1`, matching
the type returned by Xt.  It is compared using the same unsigned type before a
saved timeout is restored.

`ColorCachePropertyExists` keeps Xlib's integer request status separate from
its Boolean parse result.  X errors are checked against `Success`; the caller
receives only `True` when the bounded pixel-set payload was parsed successfully.
