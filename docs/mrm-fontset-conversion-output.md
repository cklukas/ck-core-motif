# MRM standalone font-set conversion

When a UID resource has representation type `MrmRtypeFontSet` and the caller
does not request an `XmFontList`, MRM now returns the created or cached
`XFontSet` handle.  Previously this path returned the unrelated `XFontStruct`
local variable, which could give callers an invalid value.  Font resources and
font-list conversion keep their existing public behavior.
