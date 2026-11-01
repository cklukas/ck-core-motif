#!/usr/bin/env python3
"""Check Gadget unit/direction trait getters validate widget inputs."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import textwrap


source = Path("lib/Xm/Gadget.c").read_text(encoding="utf-8")
signature = source.rindex("GetUnitType(Widget w)")
start = source.rindex("static unsigned char", 0, signature)
end = source.index("\n}", start)
unit = source[start:end + 2]
guard = "if (w == NULL || !XtIsSubclass(w, xmGadgetClass)) {"
assert guard in unit
assert "return XmPIXELS;" in unit
assert "}\n  return ((XmGadget) w)->gadget.unit_type;" in unit

compiler = shutil.which(os.environ.get("CC", "cc"))
assert compiler is not None, "a C compiler is required for the getter regression"

fixture = f"""
    #include <assert.h>
    #include <stddef.h>

    typedef void *Widget;
    struct gadget_record {{
        struct {{ unsigned char unit_type; }} gadget;
    }};
    typedef struct gadget_record *XmGadget;

    static int marker;
    static void *xmGadgetClass = &marker;
    #define XmPIXELS 0

    static int
    XtIsSubclass(Widget widget, void *widget_class)
    {{
        return widget != (Widget) 1 && widget_class == xmGadgetClass;
    }}

    {unit}

    int
    main(void)
    {{
        struct gadget_record gadget = {{ {{ 137 }} }};

        assert(GetUnitType(NULL) == XmPIXELS);
        assert(GetUnitType((Widget) 1) == XmPIXELS);
        assert(GetUnitType((Widget) &gadget) == 137);
        return 0;
    }}
"""

with tempfile.TemporaryDirectory(dir=os.environ.get("TMPDIR")) as temporary:
    root = Path(temporary)
    source_file = root / "gadget-unit-type.c"
    executable = root / "gadget-unit-type"
    source_file.write_text(textwrap.dedent(fixture), encoding="utf-8")
    subprocess.run(
        [
            compiler,
            "-std=c99",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-Werror=misleading-indentation",
            str(source_file),
            "-o",
            str(executable),
        ],
        check=True,
    )
    subprocess.run([str(executable)], check=True)

start = source.rindex("GetDirection(Widget w)")
end = source.index("\n}", start)
direction = source[start:end]
assert "w == NULL || !XtIsSubclass(w, xmGadgetClass)" in direction
assert "return XmDEFAULT_DIRECTION" in direction

print("Gadget unit/direction trait guards present; unit fallback compiles and runs")
