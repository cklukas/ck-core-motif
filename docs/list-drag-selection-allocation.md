# XmList drag selection allocation

The drag-and-drop conversion path now validates both the host `size_t` limit
and Xt's `Cardinal` allocation limit before copying selected strings.  This
keeps large-list handling from truncating the requested byte count on 64-bit
hosts while preserving the existing failure behavior.

The focused source-contract check is:

```sh
python3 tests/automated/check-list-drag-selection-allocation.py
```
