# XIM close input contract

`XmImCloseXIM` tears down the input-method state associated with a widget.
During focus or widget teardown the widget may already be absent; the
function now returns before application-context lookup when passed `NULL`.
Valid XIM cleanup is unchanged.
