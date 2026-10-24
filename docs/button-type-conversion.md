# Button-type resource conversion

The `XmRButtonType` converter now rejects empty tokens in comma-separated
resource values. Trailing, leading, and repeated commas previously left table
entries uninitialized while reporting success, which could make menu creation
depend on allocator contents. Valid tables keep the existing public values and
terminator contract.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-button-type-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
