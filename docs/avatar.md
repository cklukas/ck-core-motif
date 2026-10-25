# Avatar

`XmAvatar` is a non-interactive, additive presentation control for a person
or entity marker. It does not attach to another widget, create a global image
cache, load files or URLs, or change host geometry. Applications own its
placement, visibility, click behavior and lifetime.

The control accepts an optional borrowed `XmNavatarPixmap` and a copied
`XmNavatarInitials` fallback. A pixmap is used only while `XGetGeometry`
confirms that it is valid; an unavailable or explicitly unspecified pixmap
falls back deterministically to the copied initials. With neither value, the
accessible name is `Avatar` and the control renders an empty outlined marker.
The supplied pixmap is never freed by the widget.

`XmNavatarSize` supplies the default width and height when the application
does not provide explicit dimensions. Explicit `XmNwidth`/`XmNheight` values
remain authoritative. The control is non-interactive and exposes a label role
through the generic accessibility bridge using the initials or the `Avatar`
fallback name.

The installed consumer fixture is `tests/automated/run-avatar.py`.
