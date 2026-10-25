---
nav_exclude: true
---
# WP-13 UI-thread completion prototype

`tests/automated/check-ui-completion-prototype.c` is a consumer-side
investigation fixture.  It does not add a threading API to `libXm` and does
not make Xt widgets safe for concurrent access.

The fixture uses an ordinary POSIX pipe as a wakeup channel and
`XtAppAddInput` as the existing Xt hand-off.  Four worker threads allocate
independent result records and write only their pointers to the pipe.  The Xt
input callback validates, consumes and frees those records on the owning UI
thread.  No worker calls Xt, Xlib or a widget.

It runs two phases:

1. 320 results are posted and all are delivered through the Xt input callback;
   the callback asserts that it runs on the recorded UI thread.
2. Four workers attempt a larger batch.  The UI requests cancellation after
   the first callback, workers stop at a cancellation boundary, queued results
   are drained and freed, the input source is removed, and the descriptors and
   application context are destroyed.

The fixture uses C11 atomics for cancellation, worker-completion and allocation
failure state.  A timer/input tie is handled by draining the pipe once more on
the UI thread before teardown, so removing the input source cannot silently
leak a queued result.  The runner builds against an absolute installed prefix,
checks the resolved `libXt`, runs under Xvfb with a fresh output directory and
writes a receipt with source/output hashes and command timings.

## Decision boundary

The prototype demonstrates that existing Xt input/work-procedure mechanisms
can provide a UI-thread completion hand-off when an application owns its
result queue and wakeup lifecycle.  It does not prove portability beyond the
POSIX/X11 cohort, nor does it provide a library-owned cancellation or context
shutdown contract.  WP-14 must only proceed if consumers need a standardized
cross-application ownership and cancellation API after comparing this pattern
with their existing pipes, work procedures or event-loop integration.
