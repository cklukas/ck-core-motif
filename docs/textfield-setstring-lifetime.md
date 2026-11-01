---
nav_exclude: true
---
# TextField value callback lifetime

A value-change callback may destroy the text field, its parent or its shell.
`XmTextFieldSetString` must then stop without repainting or reading the freed
field. This also applies to the wide-character setter that delegates to it.

With multiple callbacks, immediate destruction can free Xt's compiled callback
list while `XtCallCallbackList` is still traversing it. TextField activation and
SetString value notifications now dispatch an owned snapshot obtained through
the callback resource. Dispatch stops when the field is destroyed or marked for
deferred destruction. Surviving fields retain callback order and normal default
button propagation. Changes to the registered list affect subsequent dispatches.

`run-textfield-setstring-lifetime.py` covers narrow/wide setters, immediate and
event-deferred destruction, and field/parent/shell destruction with callbacks
before and after the destructive callback. `run-textfield-activation-lifetime.py`
also checks multiple callbacks and default-button propagation. Both font configurations pass 17 setter/registration and eight activation cases. The
NumberBox editing fixture also passes all eight light/dark/scale configurations
under Valgrind with zero invalid-access errors (undefined-value and leak checks
were disabled). This does not complete the wider text-editing callback audit.


The dispatcher is the private `_XmCallCallbacksUntilDestroyed` utility in
CallbackI. It applies to callback resources of real Xt objects; it does not
replace the separate extension-record callback mechanism. It holds the
application lock, owns the callback records for the dispatch and returns False
when callers must stop accessing the object. NumberBox's value and invalid-input
notifications share this implementation. A registration-mutation case verifies
that callback removal/addition changes the next dispatch, not the current one.
