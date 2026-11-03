# Avatar temporary drawing allocation

`XmAvatar` treats every temporary X drawable, graphics context, and XRender
picture as fallible. If an allocation fails, it releases every previously
acquired temporary resource and uses the safe circular fallback where one is
available. It never sends drawing requests through `None` drawables or picture
handles.

This does not change the public `XmAvatar` API or ownership rules. It only
makes redraw under X-server resource pressure fail safely; the next expose may
render normally once the server can allocate the resource.

The focused source-contract regression is
`tests/automated/check-avatar-pixmap-allocation.py`.
