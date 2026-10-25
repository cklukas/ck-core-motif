# XmString rendition wrapping allocation

`XmStringGenerate` now checks conversion and growth while wrapping rendition
begin/end components around an unoptimized string.  If a segment conversion or
tag-array growth fails, it unlocks and returns the original string without
publishing partial rendition state.
