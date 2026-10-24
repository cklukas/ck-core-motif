# Mrm widget-resource context initialization

The resource loader now checks the context allocation used to fetch a widget
record.  Allocation failure is reported before indexed or RID lookup can use a
null context, while successful widget-resource loading retains its existing
behavior and public interfaces.

The source-level regression is
`tests/automated/check-mrm-widget-context-init.py`.
