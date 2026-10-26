# MWM action-parameter bounds

When MWM invokes an action, it records the originating client name and window
title in an optional `name=...[,title=...]` metadata string.  The builder now
keeps the length arithmetic in `size_t`, checks the `Cardinal` allocation limit,
and copies each component into the exact-sized buffer.  Oversized metadata or
allocation failure rejects the parsed action instead of allowing an integer
wrap, an undersized allocation, or a null-pointer write.
