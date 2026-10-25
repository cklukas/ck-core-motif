# BaseClass extension allocation handling

BaseClass lazily allocates per-class extension records, and `_XmPushWidgetExtData`
allocates the association node used to track temporary widget extension data.
The class-extension allocation is now checked before initialization, and the
association helper returns without dereferencing a null node when allocation
fails.  Existing successful extension setup and context ownership are
unchanged.

The regression check is `tests/automated/check-baseclass-extension-allocation.py`.
