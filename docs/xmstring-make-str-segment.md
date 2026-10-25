# XmString context segment slicing

`MakeStr` now checks each `MakeStrFromSeg` result before concatenating it.
Failed segment slices free the accumulated output and return null instead of
being treated as an optional missing operand.
