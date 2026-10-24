# WP-11 Form/hierarchy correction decision

WP-11 requires a source-backed layout defect or redundant-work hotspot before
changing `lib/Xm/Form.c`.  The WP-10 installed consumer was run twice against
the same fresh `uconsole` prefix under Xvfb.  Both result files had SHA-256
`6df8761d3757bcfa4d7751c2d38467c8764ee06aafdd2a4a67fa88df4ec3eacb` and the
same metrics:

| Observation | Count |
| --- | ---: |
| `XtMakeGeometryRequest` | 11 |
| `XtQueryGeometry` | 26 |
| `XClearArea` | 62 |
| `XCopyArea` | 125 |
| `XFillRectangle` | 209 |
| Configure events | 68 |
| Expose events | 89 |
| Map / Unmap events | 5 / 1 |
| Callback mutations | 4 |
| No-op geometry requests | 0 |
| No-op Configure events | 0 |

The corpus also completed a twenty-step resize storm, nested
`XmScrolledWindow`/`XmForm` layout, post-realization child insertion and
destruction, dynamic manage/unmanage, callback-triggered mutation and a
map/unmap cycle while preserving positive final child geometry.

## Decision

No generic `Form.c` correction is justified by the current evidence.  The
counter values show activity, but they do not identify any activity as
redundant or incorrect; the explicit no-op control is clean and the repeated
run is deterministic.  Changing attachment evaluation or pass scheduling
would therefore be speculative and could alter legacy Xt negotiation.

WP-11 is complete with **no source change** under the roadmap's explicit
completion rule.  A future correction remains possible only after a
reproducible defect or hotspot is supplied by a real client, a broader
platform/window-manager run, or a more precise toolkit instrumentation
contract.  Any such correction must rerun the WP-10 corpus and preserve final
geometry, focus order, child destruction and callback reentrancy.

This decision is intentionally not a performance claim for every Form layout,
X server or architecture.  x86_64, cyclic/conflicting attachment diagnostics,
and multi-window-manager qualification remain open evidence gaps.
