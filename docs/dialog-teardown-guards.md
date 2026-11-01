---
nav_exclude: true
---

# Legacy dialog teardown guards

`XmFileSelectionBoxGetChild`, `XmFileSelectionDoSearch`,
`XmSelectionBoxGetChild`, `XmMessageBoxGetChild`, and the public Command
helpers now return their established empty result or no-op once their parent
widget enters Xt teardown. They therefore do not expose or operate on child
widgets that may already have been destroyed.

`XmCommandAppendValue` also releases a converted but empty command string
before returning.

Run the focused source-contract regression with:

```sh
python3 tests/automated/check-dialog-teardown-guards.py
```
