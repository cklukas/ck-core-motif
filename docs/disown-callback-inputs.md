---
nav_exclude: true
---

# Selection-disown callback inputs

`DisownCallback` is registered on a widget's destruction callback while that
widget owns a selection.  Destruction and callback removal can race with
cleanup, so the callback now returns immediately when the widget pointer is
null or when its selection payload is `None`.  This keeps teardown from
dereferencing an invalid display or issuing an invalid `XtDisownSelection`
request, while preserving the normal path for a valid selection atom.

The regression check is
`tests/automated/check-disown-callback-inputs.py`.
