# Non-text Unicode qualification

The installed-prefix regression covers Unicode values through the public Label,
List, ComboBox, Menu, FileSelection, MessageBox and SelectionBox APIs. It includes a combining sequence,
a non-BMP character, Chinese Han text, a Japanese greeting, and mixed
Latin/Hebrew and Latin/Arabic labels.
It checks label round-trip, a two-segment compound `XmString` with an explicit
separator and a right-to-left segment direction, Japanese List selection and
lookup, Japanese ComboBox selected-item round-trip, a menu item label, file-selection directory
specification, message-dialog text, and selection-dialog prompt. It checks
Japanese cascade-menu labels as well. This keeps multilingual navigation and
display behavior under the same deterministic Xvfb validation used by the
other widget fixtures. It retains the existing mapped non-ASCII mnemonic
round-trip. It is still
a public-API consumer check, not a physical-keyboard or full application
journey.

The regression verifies each compound segment's payload, direction and
separator and releases the iterator buffers. List item existence, positional
lookup and match-position search cover the Japanese, Chinese Han, non-BMP and
mixed-direction items, so this does not rely only on display round-trips. Each fresh runner
result keeps its compiler and Xvfb run logs; qualification records should also
retain the exact probe and installed-library hashes for that run.

Run it with:

```sh
python3 tests/automated/run-nontext-unicode-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
