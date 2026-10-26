# Widget geometry query input contract

`XmWidgetGetBaselines` and `XmWidgetGetDisplayRect` now reject a null widget
or missing output pointer before app-context lookup and widget-class access.
They return `False` for invalid queries; valid baseline and display-rectangle
queries are unchanged.
