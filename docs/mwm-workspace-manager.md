# MWM workspace management

The default MWM build enables `MWM_WSM`, the standalone Motif workspace protocol
used by `demos/programs/workspace/wsm`. It is separate from CDE's `WSM` build.
The historical Wsm README's restriction to Motif 2.0 no longer describes this
fork.

## Protocol and window ownership

MWM advertises CONNECT, EXTENSIONS, CONFIG_FORMAT, GET_STATE, SET_STATE and
REGISTER_WINDOW on the screen's `WM_Sn` selection. Versions 1–3 are negotiated.
No optional extensions are advertised. The obsolete, unimplemented virtual-root
panning targets are not advertised or accepted.

Window attributes are `x`, `y`, `width`, `height`, `iconic`, `hidden` and `maximized`.
For maximized clients, `x`/`y`/`width`/`height` retain the saved normal
configuration; `maximized` separately records the maximized state.
Icon attributes are `x` and `y`. Coordinates and dimensions use 32-bit wire
scalars; flags use 8-bit scalars. Coordinates are interpreted as signed values
and restricted to the X11 coordinate range. Invalid dimensions are ignored.
GET_STATE returns complete snapshots even when differences are allowed.
SET_STATE accepts partial updates; unknown or destroyed client IDs are ignored.
Snapshots include transient windows and the protocol's corresponding icon IDs.

Workspace hiding unmaps the manager's frames while retaining client management
and the normal/iconic state. Transients follow their leader's visibility, and
hidden clients cannot receive keyboard focus. Geometry and iconic-state changes
retain workspace visibility. Newly managed clients are registered with Wsm;
registration replies are applied only to clients that remain managed.
If the workspace selection owner disappears or changes, MWM restores hidden
windows. Graceful Wsm exit also restores windows and removes its menu commands.

Both protocol peers validate complete message lengths, counts, attribute masks
and scalar/list widths before allocation decoding. Truncated and trailing data
are rejected. Wsm retrieves pointer-valued button IDs with pointer-sized storage
and does not retain callback-owned configuration reply structures.

Client withdrawal removes X context associations before frame/icon destructors
clear their window IDs. Queued focus events consequently cannot recover freed
client data, reinsert detached stack entries or make the stacking list circular.
Native acceptance injects stale focus events and qualifies continued resizing.

Normal termination signals notify the Xt event loop; workspace restoration and
manager cleanup run in that loop rather than inside the OS signal handler.

## Using Wsm

Build with `./configure --enable-demos`, then `make -j6` and `make install`.
Run `PREFIX/bin/mwm` on the intended X display and open Wsm through the gallery,
or run `PREFIX/share/Xm/wsm/wsm` directly. Its workspace buttons switch rooms;
its menus configure window membership, copying/linking and persistence.
Wsm stores state in `~/.wsmdb` by default; use `-file /path/to/state` for an
independent session. Each X screen has its own workspace selection.

The [acceptance record](mwm-workspace-validation.json) records the tested source
fingerprint, binary identities, commands, results and validation limits.

## Automated acceptance

The wire validator can be tested without an X server:

```sh
python3 tests/automated/run-wsm-wire-validation.py
```

The native acceptance runner requires a built source tree, Xvfb, X11 tools and Pillow for blank-capture checks.
It starts the exact MWM executable on a private display, queries its advertised
targets, exercises the real protocol and runs Wsm's actual workspace-button
callbacks and persistence path:

```sh
python3 tests/automated/run-mwm-workspace-integration.py \
  --build-root /absolute/build \
  --out /absolute/fresh/evidence
```

Use a fresh task-specific evidence directory. The Linux runner records executable identity, input hashes, commands, results
and logs, and closes the processes it started. The standalone protocol probe
returns 2 when the manager lacks required workspace targets, 1 for probe errors,
and 0 when the required target set is present.


Empty SET_STATE replies are valid selection acknowledgements, even when Xt
supplies an allocated empty value. The transfer callback preserves that result;
the protocol transport releases the delivered allocation after decoding.
Packed parameter buffers belong to UTM until it copies them into the selection
request. Menu command replies release their delivered values as well.


Wsm command identifiers use the documented multi-ID allocation path to support
consecutive allocations with modern Xlib implementations. The standalone client
advertises only its supported connect, registration and command-invocation
operations. Reconnection marks snapshot windows as managed again before later
copy/move/visibility transactions. Manager-origin geometry changes use explicit
client-root coordinates, rather than interactive move/resize offsets.


Scalar `WSMWinData` entries do not use `data_len`; packing ignores that field
for `WSM_VALUE_DATA`, while negative list lengths remain invalid. The native
protocol test explicitly sends scalar entries with an unused negative
`data_len`. This also covers request buffers reused during Wsm reconnects.
