/* Exercise the real demo startup and Mrm hierarchy, replacing only its loop. */
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <Xm/ListP.h>
#include <Xm/Frame.h>
#include <Xm/XmStringI.h>
static void CheckDialogArtwork(Widget root, Boolean modern)
{
    const char *svg_setting = getenv("PERIODIC_EXPECT_SVG");
    Boolean svg = svg_setting && !strcmp(svg_setting, "1");
    const char *widgets[] = {"*templatePick", "*infoPick", "*warningPick",
        "*errorPick", "*questionPick", "*workingPick", "*filePick",
        "*selectPick", "*promptPick"};
    const char *images[] = {"dialog_template", "dialog_information",
        "dialog_warning", "dialog_error", "dialog_question", "dialog_working",
        "dialog_file", "dialog_selection", "dialog_prompt"};
    for (Cardinal i = 0; i < XtNumber(widgets); ++i) {
        Widget item = XtNameToWidget(root, (char *)widgets[i]);
        Pixel foreground, background;
        Pixmap actual, vector;
        int depth;
        assert(item);
        XtVaGetValues(item, XmNforeground, &foreground,
            XmNbackground, &background, XmNlargeIconPixmap, &actual, NULL);
        XtVaGetValues(XtParent(item), XmNdepth, &depth, NULL);
        assert(actual != None && actual != XmUNSPECIFIED_PIXMAP);
        vector = XmGetScaledPixmap(item, (char *)images[i], foreground,
                                  background, depth, 0.0);
        assert((vector != XmUNSPECIFIED_PIXMAP) == svg);
        if (vector != XmUNSPECIFIED_PIXMAP) {
            assert(modern ? actual == vector : actual != vector);
            XmDestroyPixmap(XtScreen(item), vector);
        }
    }
}

static void VerifyApplication(XtAppContext context);
#define XtAppMainLoop VerifyApplication
#define main PeriodicMain
#include "../../demos/programs/periodic/periodic.c"
#undef main
#undef XtAppMainLoop

static void DrainLayout(XtAppContext context)
{
    unsigned events=0;
    XSync(XtDisplay(shell),False);
    while(XtAppPending(context)) {
        assert(++events<10000);
        XtAppProcessEvent(context,XtIMAll);
        XSync(XtDisplay(shell),False);
    }
}

static void CheckOverviewBounds(void)
{
    Widget table=XtNameToWidget(shell,"*workArea");
    WidgetList cells;Cardinal count;
    Dimension table_width,table_height;
    XtVaGetValues(table,XmNchildren,&cells,XmNnumChildren,&count,
        XmNwidth,&table_width,XmNheight,&table_height,NULL);
    /* The section frame is sufficient: no extra beveled wrapper around
     * the DrawingArea's own plot boundary. */
    Widget drawing=XtNameToWidget(table,"*drawArea");
    Widget drawing_frame=XtNameToWidget(table,"*drawFrame");
    assert(drawing && drawing_frame && XtParent(drawing)==drawing_frame);
    const char *aligned_frames[]={"*spinBoxFrame","*comboBoxFrame","*scrollFrame",
        "*labelFrame","*scaleFrame"};
    for(unsigned i=0;i<XtNumber(aligned_frames);++i) {
        Widget frame=XtNameToWidget(table,(char *)aligned_frames[i]);
        WidgetList parts; Cardinal n;
        Boolean found=False;
        assert(frame);
        XtVaGetValues(frame,XmNchildren,&parts,XmNnumChildren,&n,NULL);
        for(Cardinal j=0;j<n;++j) if(XmIsForm(parts[j])) {
            WidgetList controls;Cardinal nc;
            XtVaGetValues(parts[j],XmNchildren,&controls,XmNnumChildren,&nc,NULL);
            for(Cardinal k=0;k<nc;++k) if(XtIsManaged(controls[k])) {
                unsigned char top,bottom;Position y;
                XtVaGetValues(controls[k],XmNtopAttachment,&top,
                    XmNbottomAttachment,&bottom,XmNy,&y,NULL);
                assert(top==XmATTACH_FORM && bottom==XmATTACH_NONE && y==0);
                found=True;
            }
        }
        assert(found);
    }
    /* Examples demonstrate the toolkit's natural size, not a stretched cell. */
    const char *natural_controls[] = {"*pushButton", "*toggleButton"};
    for (unsigned i=0; i<XtNumber(natural_controls); ++i) {
        Widget control=XtNameToWidget(table,(char *)natural_controls[i]);
        XtWidgetGeometry preferred={0}; Dimension width,height;
        assert(control);
        XtQueryGeometry(control,NULL,&preferred);
        XtVaGetValues(control,XmNwidth,&width,XmNheight,&height,NULL);
        assert((preferred.request_mode & (CWWidth|CWHeight))==(CWWidth|CWHeight));
        if(width!=preferred.width || height!=preferred.height) {
            fprintf(stderr,"Stretched example %s: %ux%u, preferred %ux%u\n",
                XtName(control),width,height,preferred.width,preferred.height);
            abort();
        }
    }
    for(Cardinal i=0;i<count;++i) if(XmIsFrame(cells[i])) {
        Position x,y;Dimension width,height;
        WidgetList children;Cardinal child_count;
        XtVaGetValues(cells[i],XmNx,&x,XmNy,&y,XmNwidth,&width,XmNheight,&height,
            XmNchildren,&children,XmNnumChildren,&child_count,NULL);
        assert(x>=0 && y>=0 && x+width<=table_width && y+height<=table_height);
        for(Cardinal j=0;j<child_count;++j) {
            unsigned char type;
            XtVaGetValues(children[j],XmNframeChildType,&type,NULL);
            if(type==XmFRAME_TITLE_CHILD) {
                XtWidgetGeometry preferred={0};Dimension actual;
                XtQueryGeometry(children[j],NULL,&preferred);
                XtVaGetValues(children[j],XmNwidth,&actual,NULL);
                if((preferred.request_mode & CWWidth) && actual<preferred.width) {
                    fprintf(stderr,"Clipped heading %s: %u < %u\n",XtName(cells[i]),actual,preferred.width);
                    abort();
                }
            }
            if(XmIsRowColumn(children[j])) {
                Boolean radio;WidgetList buttons;Cardinal button_count;
                Dimension box_width,box_height;
                XtVaGetValues(children[j],XmNradioBehavior,&radio,XmNchildren,&buttons,
                    XmNnumChildren,&button_count,XmNwidth,&box_width,XmNheight,&box_height,NULL);
                if(!radio)continue;
                for(Cardinal k=0;k<button_count;++k) {
                    Position bx,by;Dimension bw,bh;
                    XtVaGetValues(buttons[k],XmNx,&bx,XmNy,&by,XmNwidth,&bw,XmNheight,&bh,NULL);
                    assert(bx>=0 && by>=0 && bx+bw<=box_width && by+bh<=box_height);
                }
            }
        }
    }
}

static void VerifyApplication(XtAppContext context)
{
    const char *expected = getenv("PERIODIC_EXPECT");
    Boolean historical = !strcmp(expected, "historical");
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    XClassHint hint;
    XrmValue value;
    char *type;
    const char *names[] = {"*workArea", "*pushButton", "*menuBar", "*infoDialog", "*fileDialog"};
    assert(XmGetAppearanceProfile(XtDisplay(shell), &profile));
    assert(XmGetAppearanceDensity(XtDisplay(shell), &density));
    assert(profile == (historical || !strcmp(expected, "classic") ? XmAPPEARANCE_CLASSIC :
        !strcmp(expected, "dark") ? XmAPPEARANCE_MODERN_DARK :
        !strcmp(expected, "contrast") ? XmAPPEARANCE_HIGH_CONTRAST : XmAPPEARANCE_MODERN_LIGHT));
    assert(density == (!strcmp(expected, "compact") ? XmAPPEARANCE_DENSITY_COMPACT : XmAPPEARANCE_DENSITY_COMFORTABLE));
    XSync(XtDisplay(shell), False);
    assert(XGetClassHint(XtDisplay(shell), XtWindow(shell), &hint));
    assert(!strcmp(hint.res_class, historical ? HISTORICAL_APP_CLASS : APP_CLASS));
    XFree(hint.res_name); XFree(hint.res_class);
    assert(XtIsRealized(shell));
    DrainLayout(context);
    if (!historical && profile != XmAPPEARANCE_CLASSIC) {
        Widget overview=XtNameToWidget(shell,"*overviewScroll");
        Widget main=XtParent(overview);
        Dimension width,height,main_width,main_height,margin,shadow,border;
        XtVaGetValues(overview,XmNwidth,&width,XmNheight,&height,XmNborderWidth,&border,NULL);
        XtVaGetValues(main,XmNwidth,&main_width,XmNheight,&main_height,
                      XmNmainWindowMarginWidth,&margin,XmNshadowThickness,&shadow,NULL);
        if(width+2*(margin+shadow+border)!=main_width)
            fprintf(stderr,"overview width=%u main=%u margin=%u shadow=%u border=%u height=%u/%u\n",width,main_width,margin,shadow,border,height,main_height);
        assert(width+2*(margin+shadow+border)==main_width);
        assert(height>main_height/2);
    }
    CheckDialogArtwork(shell, !historical && profile != XmAPPEARANCE_CLASSIC);
    {
        Widget surface = XtNameToWidget(shell,"*popupWindow");
        Dimension border;
        assert(surface);
        XtVaGetValues(surface,XmNborderWidth,&border,NULL);
        assert(border == (historical ? 1 : 0));
    }
    for (Cardinal i = 0; i < XtNumber(names); ++i) {
        Widget widget = XtNameToWidget(shell, (char *)names[i]);
        assert(widget != NULL);
        if (i == 1) {
            XmRenderTable table;
            XmString label;
            Dimension width, height;
            XtVaGetValues(widget, XmNrenderTable, &table, XmNlabelString, &label, NULL);
            assert(table != NULL && label != NULL);
            XmStringExtent(table, label, &width, &height);
            assert(width > 0 && height > 0);
            XmStringFree(label);
        }
        if (profile != XmAPPEARANCE_CLASSIC) {
            Pixel background;
            Colormap colormap;
            XColor actual, wanted, exact;
            const char *color = profile == XmAPPEARANCE_MODERN_DARK ? "#202124" :
                profile == XmAPPEARANCE_HIGH_CONTRAST ? "#000000" :
                i == 1 ? "#e2e6eb" : "#f1f2f4";
            if (i == 1 && !strcmp(expected, "override")) color = "#006644";
            XtVaGetValues(widget, XmNbackground, &background, XmNcolormap, &colormap, NULL);
            actual.pixel = background;
            XQueryColor(XtDisplay(widget), colormap, &actual);
            assert(XAllocNamedColor(XtDisplay(widget), colormap, color, &wanted, &exact));
            if (actual.red != wanted.red || actual.green != wanted.green || actual.blue != wanted.blue) {
                fprintf(stderr, "FAIL %s background: %04x/%04x/%04x expected %s\n",
                    names[i], actual.red, actual.green, actual.blue, color);
                abort();
            }
        }
    }
    if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) {
        Widget title = XtNameToWidget(shell, "*titleLabel");
        Boolean traversal;
        Dimension highlight;
        assert(title != NULL);
        XtVaGetValues(title, XmNtraversalOn, &traversal, XmNhighlightThickness, &highlight, NULL);
        assert(!traversal && highlight == 0);
    }
    if(profile!=XmAPPEARANCE_CLASSIC) {
        Widget notebook=XtNameToWidget(shell,"*notebook");
        Widget scroll=XtNameToWidget(shell,"*toggleButtonControlWin"),clip;
        const char *controls[]={"toggleIndOnControl","toggleTogModeControl",
            "toggleSetControl","toggleVizOffControl","toggleFillControl",
            "toggleSelColorControl","toggleUnselColorControl",
            "toggleIndTypeControl","toggleIndSizeControl"};
        double factor=!strcmp(expected,"scaled")?1.5:1.0;
        Position clip_x,clip_y,first_x=0;Dimension clip_width;
        XtVaSetValues(shell,XmNwidth,(Dimension)(1440*factor),
            XmNheight,(Dimension)(1200*factor),NULL);
        XtVaSetValues(notebook,XmNcurrentPageNumber,11,NULL);DrainLayout(context);
        XtVaGetValues(scroll,XmNclipWindow,&clip,NULL);
        XtVaGetValues(clip,XmNwidth,&clip_width,NULL);
        XtTranslateCoords(clip,0,0,&clip_x,&clip_y);
        for(unsigned n=0;n<XtNumber(controls);++n){
            char path[80];Position x,y;Dimension width;Widget control;
            snprintf(path,sizeof(path),"*%s",controls[n]);control=XtNameToWidget(shell,path);
            assert(control);XtTranslateCoords(control,0,0,&x,&y);
            XtVaGetValues(control,XmNwidth,&width,NULL);
            if(x<clip_x || x+width>clip_x+clip_width)
                fprintf(stderr,"Toggle editor %s: x=%d width=%u clip=%d width=%u\n",
                    controls[n],x,width,clip_x,clip_width);
            assert(x>=clip_x && x+width<=clip_x+clip_width);
            if(!n)first_x=x;else assert(x==first_x);
        }
        XtVaSetValues(notebook,XmNcurrentPageNumber,1,NULL);DrainLayout(context);
    }
    if(profile!=XmAPPEARANCE_CLASSIC) {
        Widget scroll=XtNameToWidget(shell,"*overviewScroll");
        Dimension table_width,table_height,again_width,again_height;
        Widget table=XtNameToWidget(shell,"*workArea");
        assert(scroll);
        XtVaSetValues(shell,XmNwidth,800,XmNheight,600,NULL);
        DrainLayout(context);CheckOverviewBounds();
        XtVaGetValues(table,XmNwidth,&table_width,XmNheight,&table_height,NULL);
        XtVaSetValues(shell,XmNwidth,1440,XmNheight,1080,NULL);
        DrainLayout(context);CheckOverviewBounds();
        XtVaSetValues(shell,XmNwidth,800,XmNheight,600,NULL);
        DrainLayout(context);CheckOverviewBounds();
        XtVaGetValues(table,XmNwidth,&again_width,XmNheight,&again_height,NULL);
        assert(table_width==again_width && table_height==again_height);
    }
    /* A legacy app-defaults file must not enter the modern resource class. */
    if (!historical)
        assert(!XrmGetResource(XrmGetDatabase(XtDisplay(shell)), "periodic.fontList",
                              APP_CLASS ".FontList", &type, &value));
    {
        Widget list = XmCreateScrolledList(XtNameToWidget(shell, "*workArea"), "geometryProbe", NULL, 0);
        XmListWidget native = (XmListWidget)list;
        XtWidgetGeometry request, reply;
        Dimension heights[] = {0, 1, 65535};
        native->primitive.shadow_thickness = 0;
        native->list.HighlightThickness = 0;
        native->list.margin_height = 0;
        native->list.spacing = 0;
        request.request_mode = CWWidth | CWHeight;
        request.width = 65535;
        request.height = 65535;
        for (Cardinal i = 0; i < XtNumber(heights); ++i) {
            native->list.MaxItemHeight = heights[i];
            (void)XtQueryGeometry(list, &request, &reply);
        }
        XtDestroyWidget(XtParent(list));
    }
    puts("PASS Periodic startup, UIL controls/menus/dialogs, overrides, bounded list queries");
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(context);
}

int main(int argc, char **argv)
{
    /* Exercise the sentinel before any other Motif call initializes its cache. */
    XmStringTag tag = _XmStringCacheTag("periodic-native-tag", XmSTRING_TAG_STRLEN);
    assert(tag != NULL && !strcmp(tag, "periodic-native-tag"));
    assert(_XmStringCacheTag("periodic-native-tag", 19) == tag);
    assert(_XmStringCacheTag(XmFONTLIST_DEFAULT_TAG, XmSTRING_TAG_STRLEN) != NULL);
    return PeriodicMain(argc, argv);
}
