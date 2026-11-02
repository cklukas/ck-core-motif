# Paned demo factories and sash controls

The generated Paned demo now gives both widget factories explicit `Widget`
parameters, allowing the current C compiler to verify their callers.  Its
client-data resource values pass through `uintptr_t`, avoiding assumptions
that a pointer and an unsigned integer have the same width.

The third pane's **Pane Demo** control is now an `XmToggleButton`, consistent
with the first two **Show Sash** controls.  It receives the same value-change
callback, so its state correctly enables or disables the third pane sash.

The focused source-contract check is:

```sh
python3 tests/automated/check-paned-demo-c23.py
```
