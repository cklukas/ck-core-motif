# CommandPalette rebuild allocation

CommandPalette filtering now allocates only the text needed to evaluate the
filter and the final compound string inserted into the list. A previously
constructed plain C string was never displayed; its allocation could therefore
make a valid command disappear on transient memory pressure. Shortcut labels
continue to be rendered through the existing `XmString` path.
