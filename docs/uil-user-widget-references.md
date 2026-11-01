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

## Using Wsm

Build with `./configure --enable-demos`, then `make -j6` and `make install`.
Run `PREFIX/bin/mwm` on the intended X display and open Wsm through the gallery,
or run `PREFIX/share/Xm/wsm/wsm` directly. Its workspace buttons switch rooms;
its menus configure window membership, copying/linking and persistence.
Wsm stores state in `~/.wsmdb` by default; use `-file /path/to/state` for an
independent session. Each X screen has its own workspace selection.

## Automated acceptance

The wire validator can be tested without an X server:

```sh
python3 tests/automated/run-wsm-wire-validation.py
```

The native acceptance runner requires a built source tree, Xvfb and X11 tools.
It starts the exact MWM executable on a private display, queries its advertised
targets, exercises the real protocol and runs Wsm's actual workspace-button
callbacks and persistence path:

```sh
python3 tests/automated/run-mwm-workspace-integration.py \
  --build-root /absolute/build \
  --out /absolute/fresh/evidence
```

On macOS, temporary outputs must be under an approved mounted temporary volume.
The Linux runner records executable identity, input hashes, commands, results
and logs, and closes the processes it started. The standalone protocol probe
returns 2 when the manager lacks required workspace targets, 1 for probe errors,
and 0 when the required target set is present.


Empty SET_STATE replies are valid selection acknowledgements, even when Xt
supplies an allocated empty value. The transfer callback preserves that result;
the protocol transport releases the delivered allocation after decoding.
Packed parameter buffers belong to UTM until it copies them into the selection
request. Menu command replies release their delivered values as well.
