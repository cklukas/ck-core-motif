# Optimized-a XmString concatenation failures

When concatenation converts an optimized left operand, conversion and the
subsequent entry-array resize can both fail.  The path now checks both results,
restores a temporarily detached segment array, and releases the owned inputs
before returning null.
