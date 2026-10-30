# ImageCache string hash distribution

The internal image-cache string hash now incorporates the final character of
the name. The previous code read the terminating NUL byte for every non-empty
name, needlessly collapsing hash entropy and increasing collisions.
