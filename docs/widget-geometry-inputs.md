# Widget geometry query input contract

`XmWidgetGetBaselines` and `XmWidgetGetDisplayRect` now reject a null widget
or missing output pointer before app-context lookup and widget-class access.
They return `False` for invalid queries. `XmWidgetGetDisplayRect` also returns
the selected primitive or gadget display-rectangle trait's result instead of
reporting success when that trait declines the query.

The focused source-level regression is
`tests/automated/check-widget-geometry-inputs.py`.
