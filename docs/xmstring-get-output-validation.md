# XmString extraction output validation

`XmStringGetNextComponent` now rejects missing output addresses before
advancing a context. `XmStringGetLtoR` likewise rejects a missing text output
pointer before initialization, returning failure without dereferencing it.

The source contract is covered by
`tests/automated/check-xmstring-get-outputs.py`.
