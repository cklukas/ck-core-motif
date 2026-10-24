# Text source range bounds

Legacy text-source conversion now checks `XmTextPosition` ranges before
allocating or passing positions through the `int`-indexed source reader.  A
range that cannot be represented safely is rejected with a null result; valid
ranges retain the existing byte and wide-character conversion behavior.

Run the native installed-prefix regression with:

```sh
python3 tests/automated/run-text-source-regressions.py /absolute/path/to/prefix /absolute/path/to/results
```
