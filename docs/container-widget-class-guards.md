---
nav_exclude: true
---
# Container widget-class guards

The public `XmContainerGetItemChildren`, `XmContainerRelayout`, and
`XmContainerReorder` helpers now reject `NULL` and non-Container widget
handles before looking up an application context, constraints, or child
storage. This keeps malformed caller input on the documented no-op path
instead of treating an unrelated widget as an `XmContainerWidget` and
dereferencing incompatible private state. Valid Container calls are
unchanged.

The source contract is checked by
`tests/automated/check-container-widget-class-guards.py`.
