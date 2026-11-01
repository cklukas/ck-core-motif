---
nav_exclude: true
---
# TextField activation callback lifetime

A TextField activation callback may destroy the field, its parent or its shell.
The action must not subsequently dereference the destroyed field to propagate
activation to its parent. This also applies when an application invokes
`XtCallActionProc(field, "activate", ...)` outside normal event dispatch, where
Xt may destroy the widgets immediately rather than deferring destruction.

The action observes destruction while dispatching callbacks, removes the
stack-backed observer before returning if the field survives, and skips parent
propagation after immediate or deferred destruction. Surviving fields retain
normal default-button activation through their parent manager.

`run-textfield-activation-lifetime.py` covers all three destruction levels and
normal parent dispatch through direct action calls and keyboard events. All
eight cases pass in both native font configurations. The NumberBox editing
fixture additionally covers destruction through a composite editor's numeric
and invalid-value callbacks. A pre-fix native backtrace confirmed the stale
parent dereference in TextField Activate / _XmParentProcess.
