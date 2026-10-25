# XmString table concatenation failure propagation

`XmStringTableToXmString` now stops immediately when concatenating a valid
table element fails.  It no longer lets the subsequent break-component step
mask that failure with a partial result.
