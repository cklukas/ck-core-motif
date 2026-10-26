# MWM DT session-hint input

`GetDtSessionHints` now returns immediately when called without a
`WmScreenData` record. This prevents a null dereference before the root-window
property lookup. Calls with valid screen data continue to parse and remove the
session-hints property as before.
