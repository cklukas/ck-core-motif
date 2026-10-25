# Non-text Unicode qualification

The installed-prefix regression covers Unicode values through the public Label,
List, ComboBox, Menu and FileSelection APIs. It includes a combining sequence,
a non-BMP character and Han text, then checks label round-trip, a two-segment
compound `XmString` with an explicit separator, List selection, ComboBox
selected-item round-trip, a menu item label and a file-selection directory
specification. This keeps multilingual navigation and display behavior under
the same deterministic Xvfb validation used by the other widget fixtures. It
is still a public-API consumer check, not a physical-keyboard or full
application journey.

The current qualified run passes 16 checks. Compound-string iteration verifies
both segments and releases the segment buffers returned by the public iterator.

Run it with:

```sh
python3 tests/automated/run-nontext-unicode-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
