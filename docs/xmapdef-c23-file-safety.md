# xmapdef file-viewer safety

The historical `xmapdef` application-defined-scroller demo now uses typed C23
function declarations for its factories, callbacks, and file-viewer helpers.
Its line-table replacement loop releases only valid old entries, closes each
successfully opened input stream, and clamps scrollbar-derived draw ranges
before indexing line storage.  A failed file-selection request also releases
the temporary Motif text string.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmapdef-c23-file-safety.py
```
