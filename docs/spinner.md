---
nav_exclude: true
---
# Spinner

`XmSpinner` is a small indeterminate activity indicator for applications that
need feedback while work is in progress. It is additive and resource-driven;
existing Motif classes and callback contracts are unchanged.

`XmNspinnerActive` controls whether the indicator is running and
`XmNspinnerInterval` selects the update period. Intervals are bounded to
10–1000 milliseconds. The control schedules one Xt timeout at a time only
while it is realized, managed, mapped, and sensitive. The timeout is cancelled
before unmap, deactivation, or destruction, and no worker thread or global
event loop is created. `XmNspinnerLabelString` provides an optional
description.

`XmNspinnerStateChangedCallback` receives an `XmSpinnerCallbackStruct` with
the normalized active state and current phase. `XmSpinnerGetPhase` is intended
for diagnostics and deterministic tests; applications normally only need the
active state.

The accessibility provider exposes a progress-bar role with the busy state and
the spinner label. It intentionally has no numeric value or actions.

`tests/automated/run-spinner.py` builds and runs the Xvfb fixture. The fixture
covers explicit dimensions, timer advancement, active-state callbacks,
deactivation/unmanagement cancellation, accessibility semantics, and teardown.
