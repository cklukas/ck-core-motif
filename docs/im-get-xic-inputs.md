# XIC creation input contract

`XmImGetXIC` creates or retrieves an input context for a widget.  A null
widget cannot identify an input-method registration, so the function now
returns `NULL` before application-context lookup.  Valid XIC creation and
reuse are unchanged.
