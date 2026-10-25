---
nav_exclude: true
---
# WP-14 UI-thread completion API decision

WP-13 tested the smallest existing-Xt pattern that provides the required
handoff: worker-owned result records, a POSIX wakeup pipe, `XtAppAddInput`,
explicit cancellation, UI-thread draining, and context teardown.  The strict
installed-prefix fixture passed with four worker threads, 320 successful phase
one deliveries, a cancellation phase that freed queued results, no invalid
records, no allocation/post failures, and no worker-side Xt/Xlib calls.

That evidence demonstrates a viable application pattern, but it does not
establish a missing toolkit contract.  The pattern's ownership and wakeup
objects remain application-owned, and a library API would have to define
cross-platform wakeups, result allocation, cancellation races, context
shutdown, and error propagation for consumers that have not been identified in
this repository.

## Decision

**No-go for a public WP-14 API in the current tranche.**  Do not add a new
completion object, worker pool, hidden thread, or process-wide queue to
`libXm`.  Applications can continue using pipes, `XtAppAddInput`, work
procedures, or their existing event-loop integration while keeping all widget
and display operations on the owning Xt thread.

Re-open WP-14 only when at least one real installed consumer supplies a
reproducible requirement that the existing pattern cannot satisfy.  That
requirement must include result ownership, cancellation, application-context
shutdown, platform scope and latency expectations.  Any future API must be
additive, keep worker code free of Xt/Xlib calls, and pass the WP-13 lifecycle
and race fixture before release.
