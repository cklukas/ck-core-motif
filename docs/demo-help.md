# Demo help dialogs

File Manager and other Xmd demos share the `XmdHelp` widget. Its notebook and
Dismiss button determine the dialog's natural size. Child resize requests must
use initialized parent dimensions and the child's requested width, height and
border width. Query-only or rejected requests leave the child unchanged;
accepted requests retain spacing between the notebook and Dismiss button.
The help manager does not derive its width from the screen width.

Help titles and page contents are compound strings and are released with
`XmStringFree` when replaced or destroyed.

## Regression validation

On macOS, with an installed toolkit and XQuartz development files:

```sh
PATH=/opt/X11/bin:$PATH python3 tests/automated/run-help-geometry.py \
  --prefix PREFIX --out TASK_TMP/help-geometry
```

Use a fresh output directory beneath a mounted project temporary volume and
set `TMPDIR` there before invoking the runner. The native test uses an isolated
Xvfb server and the File Manager help asset. It checks bounded initial geometry,
query-only requests, accepted child resizing, border spacing, rejected position
requests, reopening and destruction. Pattern-initialized automatic variables
make dependence on uninitialized geometry reproducible. The runner records
source hashes, compiler commands, elapsed times and results.
