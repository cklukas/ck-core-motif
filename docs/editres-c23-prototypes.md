# Editres C23 prototype compatibility

The Editres demonstration protocol implementation in `demos/lib/Xmd/RegEdit.c`
uses exact typed declarations for its event handlers, selection callbacks,
protocol encoders and decoders, widget-tree traversal, and resource converter.
This replaces legacy K&R definitions and empty forward declarations without
changing the wire format or the public `XmdRegisterEditres` entry point.

The implementation is therefore accepted by the project's C23 configuration.
The focused source-contract check is:

```sh
python3 tests/automated/check-regedit-c23-prototypes.py
```

The relevant demonstration object can be compiled incrementally with the
configured build using the `RegEdit.o` target.
