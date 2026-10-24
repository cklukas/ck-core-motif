# Compound-text segment lengths

Compound-text decoding now checks the combined encoding-prefix and segment
length before allocating or passing the size to Xlib. An unrepresentable
length is rejected cleanly; ordinary `XmRString` to `XmRXmString` conversion
remains unchanged.
