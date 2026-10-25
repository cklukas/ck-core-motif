# Fixed-pattern allocation bounds

The Motif pattern helper expands a slash-delimited pattern before matching.
Its size calculation now checks both native `size_t` arithmetic and the Xt
`Cardinal` size parameter, then passes the checked width explicitly. Patterns
that cannot be represented by Xt allocation APIs are rejected cleanly.
