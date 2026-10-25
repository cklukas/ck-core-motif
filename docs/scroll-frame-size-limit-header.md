# Include the header for ScrollFrameT size guards

The ScrollFrameT child-array allocation check uses `SIZE_MAX` to reject an
overflow before calculating the requested byte count.  The source now
includes `<stdint.h>` explicitly, so that guard is available regardless of
which platform headers happen to be included transitively.
