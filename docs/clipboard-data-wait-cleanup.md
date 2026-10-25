# Clipboard cut-by-name wait cleanup

`ClipboardRequestDataAndWait` now restores the root-window event mask on every
early failure path. It also refuses to create an Xt timeout when the request
window cannot map to a widget/application context, avoiding a null-context
event-loop call.
