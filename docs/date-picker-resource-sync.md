---
nav_exclude: true
---
# DatePicker resource synchronization

When an application changes the DatePicker year, month, or day resources with
`XtSetValues` while its popup calendar exists, the new date is now applied to
that calendar child as well.  The child update is performed without emitting a
second date-changed callback.

If the child cannot accept the requested date, the parent keeps its previous
date instead of displaying a state that disagrees with the popup.
