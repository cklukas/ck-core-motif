# Scrolled Text allocation bounds

`XmCreateScrolledText` builds a derived widget name and a merged argument
array on the stack.  It now checks the name length before narrowing to Xt's
`Cardinal` size and checks the merged `Arg` byte count before `XmStackAlloc`.

Oversized inputs fail before the application lock or stack allocation; normal
widget creation is unchanged.
