---
nav_exclude: true
---
# ScrollFrame navigator callback validation

`_XmSFAddNavigator` now validates the navigator trait callbacks before it
registers a navigator. A trait with no `setValue` callback cannot be
initialized, and a configured move callback cannot be installed when the
navigator does not provide `changeMoveCB`. In either case the function emits
the existing warning and leaves the navigator list unchanged.

This preserves the public Motif API while preventing a malformed or partial
trait implementation from causing a null function-pointer call during widget
creation.
