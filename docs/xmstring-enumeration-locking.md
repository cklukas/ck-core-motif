# XmString enumeration locking

`XmStringIsVoid` and `XmCvtXmStringToByteStream` now delegate synchronization
to `XmeStringGetComponent` rather than holding the process lock across repeated
component reads. This prevents recursive locking while preserving serialized
context access.
