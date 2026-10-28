# Wide text-source conversion failures

When a wide source value cannot be represented in the active multibyte
encoding, the text-source initializer keeps the source length at zero and now
also writes a terminating NUL into its allocated buffer.  Consumers therefore
see a valid empty string rather than an uninitialized buffer.
