# Non-text Unicode qualification

The installed-prefix regression covers Unicode values through the public Label,
List, ComboBox, Menu and FileSelection APIs. It includes a combining sequence,
a non-BMP character, Han text, and mixed Latin/Hebrew and Latin/Arabic labels.
It checks label round-trip, a two-segment compound `XmString` with an explicit
separator and a right-to-left segment direction, List selection and lookup,
ComboBox selected-item round-trip, a menu item label and a file-selection
directory specification. This keeps multilingual navigation and display
behavior under the same deterministic Xvfb validation used by the other widget
fixtures. It also round-trips a non-ASCII `XmNmnemonic` resource. It is still
a public-API consumer check, not a physical-keyboard or full application
journey.

The regression verifies each compound segment's payload, direction and
separator and releases the iterator buffers. List item existence, positional
lookup and match-position search cover both the non-BMP and mixed-direction
items, so this does not rely only on display round-trips. Each fresh runner
result keeps its compiler and Xvfb run logs; qualification records should also
retain the exact probe and installed-library hashes for that run.

Run it with:

```sh
python3 tests/automated/run-nontext-unicode-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
