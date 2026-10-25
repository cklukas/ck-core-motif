# Icon-directory allocation bounds

The icon-name cache copies directory prefixes into Xt-owned storage. The
directory lookup path now rejects lengths that cannot represent the trailing
NUL byte or Xt `Cardinal` allocation width, and passes the checked byte count
explicitly. Existing uncached-directory fallback behavior is preserved.
