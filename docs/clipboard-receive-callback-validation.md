# Clipboard receive-callback validation

`ClipboardReceiveData` now validates its callback context, X11 output
pointers, conversion format (8/16/32), and non-empty payload pointer before
computing byte counts. Invalid or failed conversions are marked unsuccessful
and their rejected payloads are released.
