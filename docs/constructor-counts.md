# Convenience-constructor argument counts

Scrolled Text and List constructors add internal arguments to the caller's
argument count. They now reject counts that would overflow the `Cardinal`
addition before allocating or merging argument arrays. Normal argument lists
retain the existing behavior; an overflow returns `NULL` without reading the
caller-provided array.
