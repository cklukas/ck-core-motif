# Tab-list proposal widget validation

`XmStringTableProposeTablist` uses the widget's display and unit settings while
measuring strings. It now rejects a null widget before entering the Xt display
path, matching its existing validation of the string table and empty input.
