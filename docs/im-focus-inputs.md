# Input-method focus/value input contract

`XmImSetFocusValues`, `XmImSetValues`, and `XmImUnsetFocus` now return when
their widget argument is `NULL`, before application-context lookup or widget
parent traversal.  This makes focus cleanup and optional IM updates safe
during teardown while preserving valid-widget behavior.
