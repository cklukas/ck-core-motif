# Toolbar action-array allocation failure

`XmToolbarSetActions` checks the action count before changing toolbar state, so
an oversized request is rejected without erasing the existing actions. It also
handles an `XtCalloc` failure before creating child widgets or dereferencing
the new array. Allocation failure after replacement begins retains the API's
existing semantics: the toolbar remains valid and can be populated by a later
call.

The focused source-contract check is:

```sh
python3 tests/automated/check-toolbar-allocation-failure.py
```
