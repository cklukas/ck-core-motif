# Form shadow redisplay guards

`XmForm` draws its manager shadow from both the resize path (when a shrink may
not generate an expose) and the ordinary expose path. Those paths previously
assumed that the window, manager shadow GCs, thickness, and dimensions were
already valid.

Both paths now use one prerequisite check. Invalid intermediate state skips
only the shadow request; child placement and gadget redisplay continue, and a
valid realized form retains its existing shadow geometry and type.

The focused source contract is checked by
`tests/automated/check-form-shadow-redisplay-guards.py`.
