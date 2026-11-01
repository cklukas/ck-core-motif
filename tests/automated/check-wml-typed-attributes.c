/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Use the driver's real parser globals without running its output stage. */
#include <string.h>
#define main wml_driver_main
#include "../../tools/wml/wml.c"
#undef main

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "WML typed attributes: %s\n", #test); return 1; } } while (0)

static ObjectPtr object(char *name)
{
    int index = wmlFindInHList(wml_synobj_ptr, name);
    return index < 0 ? NULL : wml_synobj_ptr->hvec[index].objptr;
}

int main(int argc, char **argv)
{
    WmlSynClassDefPtr widget;
    WmlSynDataTypeDefPtr datatype;
    WmlSynResourceDefPtr resource;
    WmlSynCharSetDefPtr charset;
    CHECK(argc == 2);
    CHECK(freopen(argv[1], "r", stdin) != NULL);
    wmlInit();
    CHECK(yyparse() == 0 && wml_err_count == 0);
    fclose(stdin);
    widget = (WmlSynClassDefPtr)object("ProbeWidget");
    datatype = (WmlSynDataTypeDefPtr)object("ProbeType");
    resource = (WmlSynResourceDefPtr)object("ProbeResource");
    charset = (WmlSynCharSetDefPtr)object("ProbeCharset");
    CHECK(widget && datatype && resource && charset);
    CHECK(widget->dialog == TRUE);
    CHECK(strcmp(widget->widgetclass, "probeWidgetClass") == 0);
    CHECK(widget->resources && widget->resources->exclude == WmlAttributeTrue);
    CHECK(datatype->xrm_support == WmlAttributeFalse);
    CHECK(strcmp(datatype->int_lit, "probe_literal") == 0);
    CHECK(resource->xrm_support == WmlAttributeFalse);
    CHECK(strcmp(resource->datatype, "ProbeType") == 0);
    CHECK(charset->direction == WmlCharSetDirectionRtoL);
    CHECK(charset->parsedirection == WmlCharSetDirectionLtoR);
    CHECK(charset->charsize == WmlCharSizeTwoByte);
    CHECK(strcmp(charset->xms_name, "PROBE-CHARSET") == 0);
    puts("PASS WML string, boolean, direction and character-size attributes");
    return 0;
}
