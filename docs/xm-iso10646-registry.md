# ISO-10646 registry detection

`_XmIsISO10646` now rejects missing display/font inputs and handles a failed
`XGetAtomName` lookup without passing a null registry name to `strcmp` or
`XFree`.  The function still recognizes both canonical case spellings when the
lookup succeeds.
