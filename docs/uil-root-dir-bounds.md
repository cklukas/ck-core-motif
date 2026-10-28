# UIL root-directory allocation bounds

UIL’s root-directory helper now checks the environment or passwd-derived path
length before converting `strlen(path) + 2` to Xt’s `Cardinal` allocation size.
It also returns failure when Xt cannot allocate the buffer, rather than
dereferencing a null pointer while appending the separator.
