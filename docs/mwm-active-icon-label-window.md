# MWM active icon-label window allocation

The large active icon label is optional and is created lazily by
`CreateActiveIconTextWindow`. If X11 cannot allocate that window, the function
now records no label parent and returns. `ShowActiveIconText` consequently
skips the optional label without issuing reparent, configure, map, or context
requests against `None`.
