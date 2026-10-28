# Accessibility D-Bus child counts

The D-Bus accessibility bridge exposes child counts through an integer
protocol field.  Native child storage uses `size_t`, so the bridge now clamps
counts above `INT_MAX` before conversion.  Ordinary counts are unchanged and
large trees no longer wrap into negative or unrelated values.
