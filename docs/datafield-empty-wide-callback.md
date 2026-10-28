# DataField empty wide callback results

`XmDataField` accepts a wide-character modify-verify callback result with a
zero length and no allocated buffer.  The callback bridge now treats that as
an empty value and avoids writing a terminator through the null pointer before
the conversion step.  Non-empty results still require a valid buffer and keep
the existing conversion and allocation behavior.
