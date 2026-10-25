# XmString segment validation

`_XmStringGetSegment` now validates the context and every required output
pointer before initializing outputs. A null context can no longer be
dereferenced while preparing the compatibility direction value.
