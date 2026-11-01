# MWM command window-list loop widths

Workspace-manager command messages carry a `CARD32` count followed by that
many window IDs.  The handlers already validate the count for payload and Xt
allocation width. Their iteration variables now use the same `CARD32` type,
so a large valid count cannot overflow a signed `int` loop counter after those
checks succeed.

The change covers include, enable, disable, rename, and remove command
handlers. Ordinary window-list order and command behavior are unchanged.

Run the focused check with:

```
python3 tests/automated/check-mwm-command-window-loop-width.py
```
