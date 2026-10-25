# XmString byte-stream length validation

`XmStringByteStreamLength` now rejects a null stream and checks header-size
arithmetic before returning the total. Invalid input or overflow returns zero;
valid streams retain their existing length calculation.
