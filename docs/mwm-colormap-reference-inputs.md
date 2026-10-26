# MWM colormap reference inputs

`AddColormapWindowReference` and `RemoveColormapWindowReference` now return
immediately for a `NULL` client record. This prevents adding a null entry to a
reference list and prevents removal from treating the list terminator as a
client. Valid client references keep their existing behavior.
