# MWM raw-bitmap cache installation

The WSM raw-bitmap helper now validates its cache name, source data, and
dimensions before allocating an `XImage`. Invalid descriptors are ignored
without changing cache state; valid built-in bitmaps retain the existing
installation path.
