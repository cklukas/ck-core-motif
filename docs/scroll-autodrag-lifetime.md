# Scrolled-window auto-drag lifetime

`XmScrolledWindow` auto-drag uses an Xt timeout to move the active scrollbar
while a drop remains over an edge region.  The timeout payload is owned by the
scrolled window, not by the scrollbar, and is retired on every exit path:

- drop-site leave;
- disabling `XmNautoDragModel`;
- scrollbar removal or destruction;
- scrolled-window destruction; and
- a value-change callback that destroys or mutates either widget.

The timeout callback marks itself active while it calls the scroll-frame
navigator protocol.  It re-checks cancellation after that protocol returns,
because application callbacks can destroy the scrollbar or its parent during
the notification.  Destroy callbacks detach the payload before Xt completes
the child destruction walk; idle payload storage is released by Xt after the
walk has unwound.  No callback path dereferences a scrollbar after its
destruction notification.

The scrollbar navigator dispatcher also snapshots its callback records before
calling application code.  This keeps Xt from iterating a callback list that a
reentrant callback has caused to be freed.  The scroll-frame navigator loop
stops when a callback marks the frame for destruction rather than visiting a
borrowed navigator list after its owner has retired it.

The native lifetime regression exercises destruction, disable, leave,
repeated-timer, and callback-triggered mutation scenarios.  Run it against an
installed build under Xvfb; repeat the process to cover allocator and event
ordering differences.  Sanitizer builds should run the same scenario set.

## Qualification record

On 2026-10-25, the installed AArch64 `uconsole` cohort ran all eight scenarios
three times under separate Xvfb displays.  Each run completed 81 checks.  A
strict consumer ASan/UBSan run (`detect_leaks=1`, `abort_on_error=1`,
`halt_on_error=1`) completed the same 81 checks with no sanitizer diagnostics.
The fixture source hash was
`24a045c0d0a640765e60cd8f4cedcd0aa57af7dd57faadbc171c422fb2ff9ae3`;
the rebuilt installed `libXm.so.8.0.0` hash was
`ff96b78d4548dab3d38f803090dbef643d73f798607e94da6a2411d769d81443` and it
was run with the pinned corrected Xt dependency.

The sanitizer run before the toolkit fix reported 360 bytes in nine allocations
from the synthesized DropSMgr clipper records.  `DestroyInfo` now retires those
internal clipper subtrees before releasing the owning drop-site tree.  The
earlier broader run also exposed a separate Xt selection-context allocation;
the pinned dependency patch now retires that context during widget-destroy and
disown ordering.  A fresh helper-built Xt prefix passes strict `abi`,
`incremental-layout` and `clipboard-lifetime` consumer fixtures.  Application
drag-and-drop ownership, Fontconfig allocations and other architectures remain
separate gates.
