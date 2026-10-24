# ClipWindow translation construction

`XmClipWindow` expands virtual key bindings into a real Xt translation table
when the class is initialized. The generated text is retained for the life of
the process because Xt keeps using the returned table. Its storage grows as
needed instead of relying on a fixed 1000-byte buffer, so longer key names or
action strings cannot overwrite adjacent state.

The installed-library widget regression constructs and realizes an
`XmScrolledWindow`, which initializes the ClipWindow path:

```sh
python3 tests/automated/run-widget-regressions.py /path/to/prefix /path/to/results
```
