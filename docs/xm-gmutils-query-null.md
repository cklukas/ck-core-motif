# Geometry reply null-safety

`XmeReplyToQueryGeometry` now returns `XtGeometryNo` for a null widget or
desired geometry record. A null intended request is treated as having no
requested width/height flags. Valid geometry negotiation remains unchanged.
