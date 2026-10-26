# Text initialization storage

`XmText` creates four small buffers during widget initialization: the first
line record, the repaint range, and the current and previous highlight lists.
The buffers are now allocated into local variables and published to the widget
only after all four allocations succeed.  If any allocation fails, all partial
allocations are released and the Xt error handler is called before text
processing continues.  This prevents a null dereference or a half-initialized
widget when the allocator cannot satisfy one of the requests.

The focused source contract is checked by
`tests/automated/check-text-initialize-storage.py`.
