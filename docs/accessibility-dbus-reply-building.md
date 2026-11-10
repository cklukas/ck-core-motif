# Accessibility D-Bus reply building

The D-Bus adapter builds each method reply and each signal with
`dbus_message_iter_append_basic`, `dbus_message_iter_open_container` and
`dbus_message_iter_close_container`. Every one of them can fail for lack of
memory. After a failure the message cannot be completed, the iterator of a
container that did not open must not be used, and the containers that did open
have to be abandoned. The adapter used to ignore all of these results, so an
out-of-memory failure produced truncated messages and appended to uninitialised
sub-iterators.

All message building now goes through a small `Build` record in
`AccessibleDBus.c`:

- Each append, open and close is one checked call that records the first failure
  and remembers the open containers. Once the build has failed, the remaining
  steps do nothing, and the closing step abandons the open containers instead of
  closing them, innermost first.
- A method reply that could not be completed is dropped and the caller gets a
  `org.freedesktop.DBus.Error.NoMemory` error. A signal that could not be
  completed is dropped; no truncated event leaves the application.
- The registration call that announces the application (`Embed`) is not sent
  when its body could not be built, and the connection is reported unavailable.
- Reading the arguments of `Properties.Set` checks each step of the argument
  iterator.

## Validation

`tests/automated/run-accessible-dbus-build.py PREFIX RESULTS` compiles the
production file into `tests/automated/check-accessible-dbus-build.c` with
AddressSanitizer and UndefinedBehaviorSanitizer and links it against an
installed sanitized toolkit built with `--enable-atspi`. It runs on a virtual X
display with a private session bus. The program replaces the three building
calls (and `dbus_message_iter_abandon_container`) with versions that count the
operations of one call and fail the Nth of them, filling the iterator of a
container that failed to open with a poison pattern. For twenty-one method and
property calls on the application root, a progress bar and the cache, and for
the signals of a change and of a removal, it repeats the call once per
operation and checks that the reply is an out-of-memory error, that no
incomplete signal is sent, that the poisoned iterator is never used and that no
container is left open. `--source-dir` runs the same program against an earlier
copy of `AccessibleDBus.c`; the earlier code fails it.
