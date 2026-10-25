# Standard conversion scalar allocation

The standard transfer converter allocates one scalar for foreground,
background, or colormap requests. Each allocation is now checked before Xt
reads the widget resource. Failure returns an empty conversion with the
appropriate type and format while releasing the application lock.

Successful conversions retain their existing values, lengths, and atom types.
