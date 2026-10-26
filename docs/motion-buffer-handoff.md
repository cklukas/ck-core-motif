# Preserve motion-buffer growth ownership

Motif batches pointer-motion events in a stack buffer and promotes it to a
heap buffer when the stack capacity is reached.  The update routine now
receives the caller's buffer pointer by reference, so that promotion and later
reallocation are visible to the event loop.  Previously the promoted pointer
was local to the helper; subsequent events continued writing through the old
stack address and could corrupt the stack.

The focused source regression is
`tests/automated/check-motion-buffer-handoff.py`.
