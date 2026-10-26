# MWM icon source validation

`MakeIconPixmap` converts a source bitmap into the colored icon pixmap used by
MWM. A failed built-in bitmap allocation can otherwise leave `None` as the
source and reach `XCopyPlane`. The function now rejects missing or zero-sized
source images before allocating or drawing the destination pixmap.
