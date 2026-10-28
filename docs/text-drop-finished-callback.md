# Text drop finished-callback ABI

Text and TextField drop handling register cleanup callbacks through
`XmeTransferAddDoneProc`. The callback type is
`XmSelectionFinishedProc(Widget, XtEnum, XmTransferDoneCallbackStruct *)`.
Both implementations now use that signature directly instead of casting a
generic Xt callback, avoiding undefined dispatch on 64-bit callers while
preserving the existing context and client-data cleanup.

The contract is covered by
`tests/automated/check-text-drop-finished-callback.py`.
