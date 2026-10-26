# MWM system-modal cursor resources

`SetupSystemModalState()` now checks both bitmap resources before calling
`XCreatePixmapCursor()`.  Failed resources are freed individually, and the
modal input window falls back to the default cursor while retaining its input
grab behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-system-modal-cursor.py
```
