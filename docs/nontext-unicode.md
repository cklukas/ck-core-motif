# Non-text Unicode qualification

The installed-prefix regression covers Unicode values through the public Label,
List and ComboBox APIs. It includes a combining sequence, a non-BMP character
and Han text, then checks label round-trip, List selection and ComboBox selected
item round-trip. This keeps multilingual navigation and display behavior under
the same deterministic Xvfb validation used by the other widget fixtures.

Run it with:

```sh
python3 tests/automated/run-nontext-unicode-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
