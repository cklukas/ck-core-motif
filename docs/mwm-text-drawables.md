# MWM text-wrapper drawables

The MWM text helpers already validate displays, graphics contexts, strings,
and active-screen state. They now also validate the target drawable/window ID,
so callers that race with frame teardown cannot pass `None` into XDrawString or
Motif string drawing APIs.
