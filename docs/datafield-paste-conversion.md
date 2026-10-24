# DataField paste conversion cleanup

DataField paste conversion now releases the temporary text list in both
single-byte and wide-character paths.  Wide-character conversion failures are
detected before the result is used as a destination offset, so malformed
locale input rejects the paste safely instead of corrupting the replacement
buffer.

The existing DataField Xvfb regression and warning-enabled DataField build
remain the native smoke checks for this path.
