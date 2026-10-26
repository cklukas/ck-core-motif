# Input-method lookup input contract

`XmImMbLookupString` now rejects a null widget or key event, negative output
capacity, and a missing output buffer when the requested capacity is nonzero.
It returns zero before application-context lookup or XIM access.  Empty
capacity with a null buffer remains valid, and normal lookup behavior is
unchanged for valid arguments.
