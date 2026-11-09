# MRM font-list conversion ownership

MRM now appends each converted font-set entry to the already constructed
`XmFontList`, rather than starting a new list for every font-set record.  A
UID font list containing multiple font sets therefore preserves all entries in
their original order.

The conversion releases each temporary `XmFontListEntry` after append, checks
entry/list creation, and frees a partially built list plus the display/font
lookup key on later conversion failures.  Successful public MRM and Xm font
list behavior is unchanged.
