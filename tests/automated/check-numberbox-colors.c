/* Explicit composite foreground inheritance without stealing child ownership. */
#include <Xm/XmAll.h>
#include <Xm/NumberBox.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static Pixel color(Widget w,const char *resource) {
    Pixel pixel;XtVaGetValues(w,resource,&pixel,NULL);return pixel;
}
static Pixel named(Display*d,const char *name) {
    XColor actual,exact;CHECK(XAllocNamedColor(d,DefaultColormap(d,DefaultScreen(d)),name,&actual,&exact));return actual.pixel;
}
static void drain(XtAppContext app,Display*d) {
    XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);
}
int main(int argc,char **argv) {
    XtToolkitInitialize();XtAppContext app=XtCreateApplicationContext();
    Display*d=XtOpenDisplay(app,NULL,"numberColors","NumberColors",NULL,0,&argc,argv);CHECK(d);
    Pixel red=named(d,"#a52a35"),blue=named(d,"#264da0"),green=named(d,"#287c38");
    XrmDatabase db=XrmGetDatabase(d);
    XrmPutStringResource(&db,"*database.editor.foreground","#287c38");
    XrmPutStringResource(&db,"*provider.foreground","#a52a35");
    XrmSetDatabase(d,db);
    const double scales[]={1,1.5,2,3};unsigned cases=0;
    for(unsigned profile=0;profile<4;++profile)for(unsigned scale=0;scale<4;++scale) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,(XmAppearanceProfile)profile));
        Widget shell=XtAppCreateShell("numberColors","NumberColors",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,NULL);
        Widget number=XmVaCreateManagedNumberBox(board,"number",XmNforeground,red,NULL);
        Widget database=XmVaCreateManagedNumberBox(board,"database",XmNforeground,red,NULL);
        Widget provider=XmVaCreateManagedNumberBox(board,"provider",NULL);
        Widget automatic=XmVaCreateManagedNumberBox(board,"automatic",NULL);
        Widget claimed=XmVaCreateManagedNumberBox(board,"claimed",NULL);
        Pixel claimed_foreground=color(claimed,XmNforeground);
        XtVaSetValues(claimed,XmNforeground,claimed_foreground,NULL);
        Widget editor=XmNumberBoxGetTextField(number),up=XtNameToWidget(number,"increment");
        Widget down=XtNameToWidget(number,"decrement"),error=XtNameToWidget(number,"validation");
        CHECK(editor && up && down && error);
        XtRealizeWidget(shell);drain(app,d);
        CHECK(color(editor,XmNforeground)==red);CHECK(color(up,XmNforeground)==red);
        CHECK(color(down,XmNforeground)==red);CHECK(color(error,XmNforeground)==red);
        CHECK(color(XmNumberBoxGetTextField(database),XmNforeground)==green);
        CHECK(color(XmNumberBoxGetTextField(provider),XmNforeground)==red);
        Pixel background=color(editor,XmNbackground);
        /* An equal-valued setter is an explicit child choice, not inheritance. */
        XtVaSetValues(editor,XmNforeground,red,NULL);
        XtVaSetValues(number,XmNforeground,blue,NULL);drain(app,d);
        CHECK(color(editor,XmNforeground)==red);CHECK(color(editor,XmNbackground)==background);
        CHECK(color(up,XmNforeground)==blue);CHECK(color(down,XmNforeground)==blue);
        CHECK(color(error,XmNforeground)==blue);
        XtVaSetValues(database,XmNforeground,blue,NULL);
        CHECK(color(XmNumberBoxGetTextField(database),XmNforeground)==green);
        /* Default children still follow palettes; inherited application values
         * and explicit child overrides survive profile/density/scale changes. */
        for(unsigned live=0;live<4;++live) {
            CHECK(XmApplyAppearanceProfile(d,(XmAppearanceProfile)live));
            CHECK(XmApplyAppearanceDensity(d,live%2?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
            CHECK(XmSetDisplayScaleOverride(d,scales[(scale+live)%4]));drain(app,d);
            CHECK(color(editor,XmNforeground)==red);CHECK(color(up,XmNforeground)==blue);
            CHECK(color(down,XmNforeground)==blue);CHECK(color(error,XmNforeground)==blue);
            CHECK(color(XmNumberBoxGetTextField(database),XmNforeground)==green);
            CHECK(color(XmNumberBoxGetTextField(provider),XmNforeground)==red);
            CHECK(color(XmNumberBoxGetTextField(automatic),XmNforeground)==color(automatic,XmNforeground));
            CHECK(color(XmNumberBoxGetTextField(claimed),XmNforeground)==claimed_foreground);
        }
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS numberbox colors: %u configurations\n",cases);return 0;
}
