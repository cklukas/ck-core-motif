# MWM named-icon metadata

Named icon pixmaps require dimensions and depth from `XmeGetPixmapData` before
they can be converted. If that query fails, `MakeNamedIconPixmap` marks the
named pixmap unavailable and follows the existing built-in-icon fallback path.
