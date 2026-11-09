# Extended List demo callback contracts

The Extended List demo now gives its remaining generated callbacks typed Xt
signatures and converts the small player identifier through `intptr_t` when it
is stored in or read from the `XtPointer` row-data field.  This keeps the
example valid on pointer-width-aware C toolchains.

The **Toggle Finder** control now listens for the toggle's value-change
notification rather than its arm notification.  The finder visibility
therefore follows the newly selected state, instead of lagging one click
behind.

The focused source-contract check is:

```sh
python3 tests/automated/check-ext18list-demo-c23.py
```
