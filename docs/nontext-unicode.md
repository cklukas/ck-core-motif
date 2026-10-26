# Non-text Unicode qualification

The installed-prefix regression covers Unicode values through the public Label,
List, ComboBox, Menu and FileSelection APIs. It includes a combining sequence,
a non-BMP character and Han text, then checks label round-trip, a two-segment
compound `XmString` with an explicit separator, List selection, ComboBox
selected-item round-trip, a menu item label and a file-selection directory
specification. This keeps multilingual navigation and display behavior under
the same deterministic Xvfb validation used by the other widget fixtures. It
also round-trips a non-ASCII `XmNmnemonic` resource. It is still a public-API
consumer check, not a physical-keyboard or full application journey.

The current qualified run passes 25 checks. Compound-string iteration verifies
both segment payloads, the explicit separator and releases the segment buffers
returned by the public iterator. List item existence, positional lookup and
match-position search also use the non-BMP item, so this does not rely only on
display round-trips. Three fresh AArch64 `uconsole` Xvfb runs used the same
probe source hash (`60c94b2bd788605485046e7f95902d97b35e4f898c007f6138d82e5f31a7af3c`)
and `libXm.so.8.0.0` hash
(`2c0e75e58ec0c0e2a0cd6585f06ee4a695f54f79633b307ad47405ba0a703fa3`).

The current source probe was also run against the refreshed rpi4 static cohort;
it passes 32 checks, including ToggleSwitch and SegmentedControl label paths.
The probe source hash is
`c4efa2fa3b4f34e28e2220992a155acdec4f5a05398ea11ce84d15038d4888d4`, and the
static archive hash is
`c7a0f0b6932d1f5d07e329666f85281f328d3fc4310110de4f81ef91a94272e9`.

Run it with:

```sh
python3 tests/automated/run-nontext-unicode-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
