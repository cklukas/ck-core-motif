# Color database final lines

Color database files may end without a newline.  The name scanner now treats
that as a valid line terminator instead of dereferencing a null result from
`strchr` and crashing while loading the selector.
