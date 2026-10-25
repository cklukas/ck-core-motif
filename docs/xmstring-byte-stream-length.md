---
nav_exclude: true
---
# XmString byte-stream length validation

`XmStringByteStreamLength` now returns zero for a null input and checks header
size arithmetic before adding it to the decoded payload length. Valid byte
streams retain their existing length result.
