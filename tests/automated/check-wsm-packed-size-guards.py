#!/usr/bin/env python3
"""Ensure WSM packed-size helpers compile and reject invalid lengths."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import textwrap


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
signature = "AddPackedSize(long *size, size_t amount)"
assert signature in text
assert "amount > (size_t)INT_MAX - (size_t)*size" in text
start = text.rindex("static Boolean\n" + signature)
end = text.index("\n}", start) + 2
helper = text[start:end]
win_data = text[text.index("WinDataSizePacked(Display *dpy"):text.index("/************************************************************", text.index("WinDataSizePacked(Display *dpy"))]
assert "win_data->data_len < 0" in win_data
assert "!AddPackedSize(&size" in win_data
assert "register int i;\n    long size;" in win_data
assert "WinInfoSizePacked" in text and "!AddPackedSize(&size, (size_t)win_size)" in text
assert "request->wm_windows.num_window_properties" in text
assert "WSMStringLength(request->extensions.extension_suggestions[i])" in text

compiler = shutil.which(os.environ.get("CC", "cc"))
assert compiler is not None, "a C compiler is required for the WSM size regression"
fixture = f"""
    #include <assert.h>
    #include <limits.h>
    #include <stddef.h>

    typedef int Boolean;
    #define True 1
    #define False 0

    {helper}

    int
    main(void)
    {{
        long size = INT_MAX - 2;

        assert(AddPackedSize(&size, 2));
        assert(size == INT_MAX);
        assert(!AddPackedSize(&size, 1));
        assert(!AddPackedSize(NULL, 1));
        size = -1;
        assert(!AddPackedSize(&size, 0));
        return 0;
    }}
"""
with tempfile.TemporaryDirectory(dir=os.environ.get("TMPDIR")) as temporary:
    root = Path(temporary)
    source = root / "wsm-packed-size.c"
    executable = root / "wsm-packed-size"
    source.write_text(textwrap.dedent(fixture), encoding="utf-8")
    subprocess.run(
        [compiler, "-std=c99", "-Wall", "-Wextra", "-Werror",
         "-Wincompatible-pointer-types", str(source), "-o", str(executable)],
        check=True,
    )
    subprocess.run([str(executable)], check=True)

print("WSM packed-size arithmetic is checked and compiles with long packet sizes")
