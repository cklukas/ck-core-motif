# XmMainWindow API null-safety

`XmMainWindowSetAreas` now returns for a null main-window widget, and
`XmMainWindowSep1`, `XmMainWindowSep2`, and `XmMainWindowSep3` return `NULL`
before app-context lookup for null widgets. Valid area updates and separator
lookups remain unchanged.
