/* Exercise the real Mrm hierarchy and registered menu callbacks on Xvfb.
 * File/clipboard state is checked independently of the implementation. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <assert.h>
#include <X11/Intrinsic.h>
#include <X11/extensions/XTest.h>
#include <Xm/XmAll.h>
#include "../../demos/programs/periodic/periodic_actions.c"
static void CheckMenus(XtAppContext);
#define XtAppMainLoop CheckMenus
#define main PeriodicMain
#include "../../demos/programs/periodic/periodic.c"
#undef main
#undef XtAppMainLoop
static unsigned checks;
static void drain(void)
{
    XSync(XtDisplay(shell),False);
    unsigned limit=0;
    for(unsigned idle=0;idle<3;) {
        if(XtAppPending(appContext)) {assert(++limit<20000);XtAppProcessEvent(appContext,XtIMAll);idle=0;}
        else {usleep(5000);idle++;}
        XSync(XtDisplay(shell),False);
    }
}
static Widget item(const char *name)
{ char path[128];snprintf(path,sizeof(path),"*%s",name);Widget w=XtNameToWidget(shell,path);assert(w);return w; }
static void invoke(const char *name)
{
    Widget w=item(name);printf("ACTION %s\n",name);fflush(stdout);
    const char *resource=XmIsToggleButton(w)?XmNvalueChangedCallback:XmNactivateCallback;
    assert(XtHasCallbacks(w,resource)==XtCallbackHasSome);
    if(XmIsToggleButton(w)){XmToggleButtonSetState(w,True,False);XmToggleButtonCallbackStruct cb={.reason=XmCR_VALUE_CHANGED,.set=True};XtCallCallbacks(w,resource,&cb);}
    else XtCallCallbacks(w,resource,NULL);
    drain();checks++;
}
static void text_is(Widget w,const char *expected)
{ char *s=XmTextGetString(w);if(strcmp(s,expected))fprintf(stderr,"text %s: [%s] expected [%s]\n",XtName(w),s,expected);assert(!strcmp(s,expected));XtFree(s);checks++; }
static void label_has(Widget w,const char *expected)
{ XmString value;char *s=NULL;XtVaGetValues(w,XmNlabelString,&value,NULL);assert(XmStringGetLtoR(value,XmFONTLIST_DEFAULT_TAG,&s));if(!strstr(s,expected))fprintf(stderr,"label %s: %s expected %s\n",XtName(w),s,expected);assert(strstr(s,expected));XtFree(s);XmStringFree(value);checks++; }
static void help_has(const char *expected)
{ char *s=XmTextGetString(item("helpText"));assert(strstr(s,expected));XtFree(s);XtUnmanageChild(item("periodicHelpDialog"));checks++;drain(); }
static void file_result(const char *path)
{
    XmFileSelectionBoxCallbackStruct cb={0};cb.reason=XmCR_OK;cb.value=XmStringCreateLocalized((char *)path);
    XtCallCallbacks(item("fileDialog"),XmNokCallback,&cb);XmStringFree(cb.value);drain();
}
static void file_is(const char *path,const char *expected)
{ FILE *f=fopen(path,"rb");char b[8192];assert(f);size_t n=fread(b,1,sizeof(b)-1,f);assert(!ferror(f));fclose(f);b[n]=0;assert(!strcmp(b,expected));checks++; }
static void answer(const char *dialog,int child)
{Widget w=XmMessageBoxGetChild(item(dialog),child);XtCallCallbacks(w,XmNactivateCallback,NULL);drain();}
static void click(Widget w)
{
    Position x,y;Dimension width,height;XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);
    Widget target=w;Position local_x=width/2,local_y=height/2;
    if(!XtIsWidget(w)){Position gx,gy;XtVaGetValues(w,XmNx,&gx,XmNy,&gy,NULL);local_x+=gx;local_y+=gy;target=XtParent(w);}
    int root_x,root_y;Window child;assert(XTranslateCoordinates(XtDisplay(target),XtWindow(target),RootWindowOfScreen(XtScreen(target)),local_x,local_y,&root_x,&root_y,&child));x=(Position)root_x;y=(Position)root_y;
    XTestFakeMotionEvent(XtDisplay(w),DefaultScreen(XtDisplay(w)),x,y,0);
    XTestFakeButtonEvent(XtDisplay(w),1,True,0);drain();
    XTestFakeButtonEvent(XtDisplay(w),1,False,0);drain();
}
static Widget find_option(Widget parent)
{
    if(XmIsRowColumn(parent)){unsigned char type;XtVaGetValues(parent,XmNrowColumnType,&type,NULL);if(type==XmMENU_OPTION)return parent;}
    if(XtIsComposite(parent)){WidgetList children;Cardinal count;XtVaGetValues(parent,XtNchildren,&children,XtNnumChildren,&count,NULL);for(Cardinal i=0;i<count;i++){Widget found=find_option(children[i]);if(found)return found;}}
    return NULL;
}
static unsigned inventory(Widget menu,Boolean option)
{
    WidgetList children;Cardinal count;unsigned total=0;XtVaGetValues(menu,XtNchildren,&children,XtNnumChildren,&count,NULL);
    for(Cardinal i=0;i<count;i++){
        Widget w=children[i],sub=NULL;
        if(XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w)){XtVaGetValues(w,XmNsubMenuId,&sub,NULL);assert(sub);total+=inventory(sub,False);}
        else if(XmIsPushButton(w) || XmIsToggleButton(w)){
            const char *callback=XmIsToggleButton(w)?XmNvalueChangedCallback:XmNactivateCallback;
            assert(option || XtHasCallbacks(w,callback)==XtCallbackHasSome);total++;
        }
    }
    return total;
}
static void wait_print(void)
{
    for(unsigned i=0;i<500 && periodic_actions->print_pid;i++) {drain();usleep(10000);}
    drain();assert(!periodic_actions->print_pid);checks++;
}
static void check_focus_ring(Widget arrow)
{
    XmAppearanceProfile profile;assert(XmGetAppearanceProfile(XtDisplay(arrow),&profile));
    if(profile!=XmAPPEARANCE_MODERN_LIGHT && profile!=XmAPPEARANCE_MODERN_DARK)return;
    Dimension width,height;Pixel accent;XtVaGetValues(arrow,XmNwidth,&width,XmNheight,&height,XmNhighlightColor,&accent,NULL);
    XImage *im=XGetImage(XtDisplay(arrow),XtWindow(arrow),0,0,width,height,AllPlanes,ZPixmap);assert(im);
    size_t total=(size_t)width*height,count=0,reached=0;
    unsigned char *seen=calloc(total,1);size_t *queue=malloc(total*sizeof(*queue));assert(seen && queue);
    for(unsigned y=0;y<height;y++)for(unsigned x=0;x<width;x++)if(XGetPixel(im,x,y)==accent){if(!count){queue[0]=(size_t)y*width+x;seen[queue[0]]=1;reached=1;}count++;}
    for(size_t head=0;head<reached;head++){
        int x=queue[head]%width,y=queue[head]/width;
        for(int dy=-1;dy<=1;dy++)for(int dx=-1;dx<=1;dx++){
            int nx=x+dx,ny=y+dy;if(nx<0 || ny<0 || nx>=width || ny>=height)continue;
            size_t i=(size_t)ny*width+nx;if(!seen[i] && XGetPixel(im,nx,ny)==accent){seen[i]=1;queue[reached++]=i;}
        }
    }
    assert(count && reached==count);free(queue);free(seen);XDestroyImage(im);checks++;
}
static void CheckMenus(XtAppContext context)
{
    (void)context;setvbuf(stdout,NULL,_IONBF,0);drain();
    PeriodicActions *s=periodic_actions;assert(s);
    unsigned terminals=inventory(item("menuBar"),False)+inventory(item("popupMenu"),False)+inventory(XtParent(item("templateFileCascade")),False)+inventory(XtParent(item("filterCascade")),False);
    Widget option=find_option(item("optionFrame")),weekdays=NULL;assert(option);XtVaGetValues(option,XmNsubMenuId,&weekdays,NULL);assert(weekdays);assert(inventory(weekdays,True)==7);
    printf("INVENTORY %u callback actions plus 7 built-in weekday choices\n",terminals);assert(terminals==45);
    /* Physical menu interaction at a nonzero root position. */
    XtVaSetValues(shell,XtNx,120,XtNy,80,NULL);drain();
    click(item("optionsCascade"));
    Widget menu=XtParent(item("caseButton"));assert(XtIsManaged(menu));
    click(item("caseButton"));assert(s->case_sensitive);label_has(item("findStatus"),"1 match");
    XmToggleButtonSetState(item("caseButton"),False,True);drain();label_has(item("findStatus"),"3 matches");
    XmTextSetString(s->editor_text,"Äpfel äpfel ÄPFEL");XmTextFieldSetString(s->query,"äpfel");drain();label_has(s->find_status,"3 matches");
    invoke("caseButton");label_has(s->find_status,"1 match");
    XmToggleButtonSetState(item("caseButton"),False,True);
    XmTextSetString(s->editor_text,"A long line of text repeated to demonstrate word wrapping at the edge of the document editor. A long line of text repeated to demonstrate word wrapping at the edge of the document editor. A long line of text repeated to demonstrate word wrapping at the edge of the document editor.");
    char *original=XmTextGetString(s->editor_text);Boolean wrap,horizontal;
    XmToggleButtonSetState(item("wrapButton"),False,True);drain();
    XtVaGetValues(s->editor_text,XmNwordWrap,&wrap,XmNscrollHorizontal,&horizontal,NULL);assert(!wrap && !horizontal);
    Position nowrap_x,nowrap_y,wrapped_x,wrapped_y;assert(XmTextPosToXY(s->editor_text,40,&nowrap_x,&nowrap_y));
    invoke("wrapButton");XtVaGetValues(s->editor_text,XmNwordWrap,&wrap,XmNscrollHorizontal,&horizontal,NULL);assert(wrap);
    assert(XmTextPosToXY(s->editor_text,200,&wrapped_x,&wrapped_y));assert(wrapped_y>nowrap_y);text_is(s->editor_text,original);XtFree(original);
    /* Clipboard editing and both menu surfaces use the actual text selection. */
    XmTextSetString(s->editor_text,"alpha beta");s->editing=&s->document;s->document.last=s->editor_text;
    XmTextSetSelection(s->editor_text,0,5,XtLastTimestampProcessed(XtDisplay(shell)));invoke("copyButton");
    {char data[1024];unsigned long n=0;long private_id;int rc=XmClipboardRetrieve(XtDisplay(shell),XtWindow(s->editor_text),"UTF8_STRING",data,sizeof(data)-1,&n,&private_id);data[n<sizeof(data)?n:0]=0;assert(rc==XmClipboardSuccess && n==5 && !memcmp(data,"alpha",5));}
    XmTextFieldSetString(s->query,"");assert(XmTextFieldPaste(s->query));drain();char *field=XmTextFieldGetString(s->query);assert(!strcmp(field,"alpha"));XtFree(field);
    XmTextClearSelection(s->editor_text,CurrentTime);XmTextSetInsertionPosition(s->editor_text,10);invoke("pasteButton");text_is(s->editor_text,"alpha betaalpha");
    invoke("undoButton");text_is(s->editor_text,"alpha beta");
    XmTextSetSelection(s->editor_text,0,5,CurrentTime);invoke("cutButton");text_is(s->editor_text," beta");invoke("popupUndoButton");text_is(s->editor_text,"alpha beta");
    XmTextSetSelection(s->editor_text,5,10,CurrentTime);invoke("deleteButton");text_is(s->editor_text,"alpha");invoke("undoButton");
    invoke("clearButton");text_is(s->editor_text,"");invoke("undoButton");text_is(s->editor_text,"alpha beta");
    XmTextSetSelection(s->editor_text,0,5,CurrentTime);invoke("popupCopyButton");invoke("popupCutButton");text_is(s->editor_text," beta");
    XmTextClearSelection(s->editor_text,CurrentTime);XmTextSetInsertionPosition(s->editor_text,0);invoke("popupPasteButton");text_is(s->editor_text,"alpha beta");
    XmTextSetSelection(s->editor_text,0,5,CurrentTime);invoke("popupDeleteButton");text_is(s->editor_text," beta");invoke("popupClearButton");text_is(s->editor_text,"");
    /* Focus / activate each arrow, then change all four directions. */
    XtUnmanageChild(s->editor);drain();
    const char *directions[]={"upDirButton","downDirButton","leftDirButton","rightDirButton"};
    unsigned char values[]={XmARROW_UP,XmARROW_DOWN,XmARROW_LEFT,XmARROW_RIGHT};
    for(unsigned a=1;a<=4;a++) {char name[32];snprintf(name,sizeof(name),"arrowB%u",a);Widget arrow=item(name);XSetInputFocus(XtDisplay(shell),XtWindow(arrow),RevertToParent,CurrentTime);assert(XmProcessTraversal(arrow,XmTRAVERSE_CURRENT));drain();click(arrow);assert(s->arrow==arrow);check_focus_ring(arrow);
        for(unsigned d=0;d<4;d++){invoke(directions[d]);unsigned char value;XtVaGetValues(arrow,XmNarrowDirection,&value,NULL);assert(value==values[d]);}}
    /* Save/Open/unsaved/overwrite paths, checking bytes independently. */
    char path[2048],second[2048],included[2048],draft[2048];const char *tmp=getenv("TMPDIR");assert(tmp);
    snprintf(path,sizeof(path),"%s/document.txt",tmp);snprintf(second,sizeof(second),"%s/copy.txt",tmp);snprintf(included,sizeof(included),"%s/include.txt",tmp);snprintf(draft,sizeof(draft),"%s/draft.txt",tmp);
    FILE *f=fopen(included,"w");assert(f);fputs("included café\n",f);fclose(f);
    XmTextSetString(s->editor_text,"Document café Ω\n");invoke("saveAsButton");file_result(path);file_is(path,"Document café Ω\n");
    XmTextSetString(s->editor_text,"Changed document\n");invoke("saveButton");file_is(path,"Changed document\n");
    invoke("saveAsButton");file_result(path);assert(XtIsManaged(s->overwrite));answer("periodicOverwriteDialog",XmDIALOG_CANCEL_BUTTON);file_is(path,"Changed document\n");
    file_result(second);file_is(second,"Changed document\n");
    XmTextSetString(s->editor_text,"Replace content\n");invoke("saveAsButton");file_result(path);answer("periodicOverwriteDialog",XmDIALOG_OK_BUTTON);file_is(path,"Replace content\n");
    XmTextSetString(s->editor_text,"Unsaved text");invoke("newButton");assert(XtIsManaged(s->unsaved));
    Widget cancel=XmMessageBoxGetChild(s->unsaved,XmDIALOG_CANCEL_BUTTON);XSetInputFocus(XtDisplay(shell),XtWindowOfObject(cancel),RevertToParent,CurrentTime);drain();assert(XmProcessTraversal(cancel,XmTRAVERSE_CURRENT));drain();KeyCode help_key=XKeysymToKeycode(XtDisplay(shell),XK_F1);XTestFakeKeyEvent(XtDisplay(shell),help_key,True,0);XTestFakeKeyEvent(XtDisplay(shell),help_key,False,0);drain();assert(XtIsManaged(s->unsaved));text_is(s->editor_text,"Unsaved text");KeyCode cancel_key=XKeysymToKeycode(XtDisplay(shell),XK_Escape);XTestFakeKeyEvent(XtDisplay(shell),cancel_key,True,0);XTestFakeKeyEvent(XtDisplay(shell),cancel_key,False,0);drain();assert(!XtIsManaged(s->unsaved));text_is(s->editor_text,"Unsaved text");
    invoke("newButton");answer("periodicUnsavedDialog",XmDIALOG_HELP_BUTTON);text_is(s->editor_text,"");
    invoke("openButton");file_result(included);text_is(s->editor_text,"included café\n");
    XmTextSetString(s->editor_text,"Save before new");invoke("newButton");answer("periodicUnsavedDialog",XmDIALOG_OK_BUTTON);file_is(included,"Save before new");text_is(s->editor_text,"");
    invoke("openButton");file_result("/does/not/exist");assert(XtIsManaged(s->file));text_is(s->editor_text,"");help_has("Cannot open file");
    XtCallCallbacks(s->file,XmNcancelCallback,NULL);drain();
    invoke("saveAsButton");file_result("/does/not/exist/document.txt");assert(XtIsManaged(s->file));help_has("Cannot save file");XtCallCallbacks(s->file,XmNcancelCallback,NULL);
    char binary[2048];snprintf(binary,sizeof(binary),"%s/binary.txt",tmp);f=fopen(binary,"wb");assert(f);fwrite("a\0b",1,3,f);fclose(f);
    invoke("openButton");file_result(binary);text_is(s->editor_text,"");help_has("not valid text");XtCallCallbacks(s->file,XmNcancelCallback,NULL);
    assert(chmod(path,0444)==0);invoke("saveAsButton");file_result(path);answer("periodicOverwriteDialog",XmDIALOG_OK_BUTTON);help_has("Cannot save read-only file");file_is(path,"Replace content\n");assert(chmod(path,0644)==0);XtCallCallbacks(s->file,XmNcancelCallback,NULL);
    /* File filter menu and extra selection/merge buttons. */
    XtManageChild(s->file);PeriodicPrepareSampleDialog(s,s->file);
    const char *filters[]={"audioButton","graphicsButton","textButton"};const char *patterns[]={"*.wav","*.png","*.txt"};
    for(unsigned i=0;i<3;i++){invoke(filters[i]);XmString v;char *p=NULL;XtVaGetValues(s->file,XmNpattern,&v,NULL);assert(XmStringGetLtoR(v,XmFONTLIST_DEFAULT_TAG,&p));assert(!strcmp(p,patterns[i]));XmStringFree(v);XtFree(p);}
    XmString filename=XmStringCreateLocalized(included);XtVaSetValues(s->file,XmNtextString,filename,NULL);XmStringFree(filename);invoke("fileMergeButton");text_is(s->editor_text,"Save before new");
    XtManageChild(s->file);invoke("fileClearButton");XmString empty;char *empty_text=NULL;XtVaGetValues(s->file,XmNtextString,&empty,NULL);assert(XmStringGetLtoR(empty,XmFONTLIST_DEFAULT_TAG,&empty_text));assert(!*empty_text);XmStringFree(empty);XtFree(empty_text);XtUnmanageChild(s->file);
    /* Template actions stay entirely local. */
    XtManageChild(item("templateDialog"));XmTextSetString(item("templateText"),"hello typoqq");invoke("sendButton");text_is(s->preview_text,"hello typoqq");assert(s->message_preview && !XtIsManaged(XtParent(s->printer)));
    invoke("draftButton");file_result(draft);file_is(draft,"hello typoqq");
    invoke("includeButton");file_result(included);char *template_text=XmTextGetString(item("templateText"));assert(strstr(template_text,"Save before new"));XtFree(template_text);
    invoke("spellButton");help_has("typoqq");
    invoke("templateIncludeButton");XtCallCallbacks(s->file,XmNcancelCallback,NULL);invoke("templateDraftButton");file_is(draft,s->template_document.text);
    invoke("templateSpellButton");help_has("typoqq");invoke("cancelButton");assert(!XtIsManaged(item("templateDialog")));
    /* Print uses a fake lp installed by the runner; never a real printer. */
    invoke("printButton");text_is(s->preview_text,s->document.text);assert(!s->message_preview && XtIsManaged(XtParent(s->printer)));
    XmTextFieldSetString(s->printer,"Test printer ; no shell");invoke("submitPrint");wait_print();help_has("fake print accepted");
    char capture[2048];snprintf(capture,sizeof(capture),"%s/printed.txt",tmp);file_is(capture,s->document.text);
    setenv("PERIODIC_TEST_LP_FAIL","1",1);invoke("submitPrint");wait_print();help_has("fake print failure");unsetenv("PERIODIC_TEST_LP_FAIL");
    setenv("PERIODIC_TEST_LP_WAIT","1",1);invoke("submitPrint");assert(s->print_pid>0);invoke("cancelPrint");wait_print();unsetenv("PERIODIC_TEST_LP_WAIT");XtUnmanageChild(s->help);
    /* Calendar ranges and window arrangements produce different results. */
    const char *views[]={"dayButton","weekButton","monthButton","yearButton"};
    for(unsigned i=0;i<4;i++){invoke(views[i]);int page;XtVaGetValues(s->calendar_book,XmNcurrentPageNumber,&page,NULL);assert(page==(int)i+1);assert(XtIsManaged(s->calendar));assert(XmToggleButtonGetState(item(views[i])));}
    XtManageChild(item("infoDialog"));XtManageChild(item("warningDialog"));drain();invoke("stackButton");Position sx,sy,tx,ty;XtVaGetValues(XtParent(item("warningDialog")),XtNx,&sx,XtNy,&sy,NULL);invoke("tileButton");XtVaGetValues(XtParent(item("warningDialog")),XtNx,&tx,XtNy,&ty,NULL);assert(sx!=tx || sy!=ty);
    invoke("windowButton");help_has("File and Edit");invoke("tutorButton");assert(!XtIsSensitive(s->tutorial_back));
    for(unsigned i=0;i<4;i++)invoke("tutorialNext");assert(!XtIsSensitive(s->tutorial_next));invoke("tutorialBack");assert(XtIsSensitive(s->tutorial_next));
    invoke("versionButton");help_has(XmVERSION_STRING);
    XtUnmanageChild(s->editor);XtUnmanageChild(s->calendar);XtUnmanageChild(s->preview);XtUnmanageChild(item("infoDialog"));XtUnmanageChild(item("warningDialog"));drain();
    invoke("contextButton");Widget arrow=item("arrowB1");assert(XmProcessTraversal(arrow,XmTRAVERSE_CURRENT));drain();click(arrow);assert(!s->context_mode);help_has("ArrowButton");
    XSetInputFocus(XtDisplay(shell),XtWindow(arrow),RevertToParent,CurrentTime);drain();invoke("contextButton");KeyCode escape=XKeysymToKeycode(XtDisplay(shell),XK_Escape);XTestFakeKeyEvent(XtDisplay(shell),escape,True,0);XTestFakeKeyEvent(XtDisplay(shell),escape,False,0);drain();assert(!s->context_mode);
    /* Built-in option-menu selection changes both history and displayed text. */
    assert(XmProcessTraversal(XmOptionButtonGadget(option),XmTRAVERSE_CURRENT));drain();
    WidgetList days;Cardinal day_count;XtVaGetValues(weekdays,XtNchildren,&days,XtNnumChildren,&day_count,NULL);assert(day_count==7);
    for(Cardinal i=0;i<day_count;i++){
        XTestFakeKeyEvent(XtDisplay(shell),escape,True,0);XTestFakeKeyEvent(XtDisplay(shell),escape,False,0);drain();
        Widget trigger=XmOptionButtonGadget(option);Position gx,gy;Dimension gw,gh;XtVaGetValues(trigger,XmNx,&gx,XmNy,&gy,XmNwidth,&gw,XmNheight,&gh,NULL);int x,y;Window child;
        assert(XTranslateCoordinates(XtDisplay(shell),XtWindow(option),RootWindowOfScreen(XtScreen(shell)),gx+gw/2,gy+gh/2,&x,&y,&child));
        XTestFakeMotionEvent(XtDisplay(shell),DefaultScreen(XtDisplay(shell)),x,y,0);XTestFakeButtonEvent(XtDisplay(shell),1,True,0);drain();
        XtVaGetValues(days[i],XmNwidth,&gw,XmNheight,&gh,NULL);assert(XTranslateCoordinates(XtDisplay(shell),XtWindow(days[i]),RootWindowOfScreen(XtScreen(shell)),gw/2,gh/2,&x,&y,&child));
        XTestFakeMotionEvent(XtDisplay(shell),DefaultScreen(XtDisplay(shell)),x,y,0);drain();XTestFakeButtonEvent(XtDisplay(shell),1,False,0);drain();Widget selected=NULL;XtVaGetValues(option,XmNmenuHistory,&selected,NULL);assert(selected==days[i]);XmString expected,actual;XtVaGetValues(days[i],XmNlabelString,&expected,NULL);XtVaGetValues(XmOptionButtonGadget(option),XmNlabelString,&actual,NULL);assert(XmStringCompare(expected,actual));XmStringFree(expected);XmStringFree(actual);checks++;}
    /* Font actions already have the separate size/family/layout regression. */
    invoke("smallFontButton");invoke("largeFontButton");invoke("mediumFontButton");
    invoke("exitButton");assert(XtIsManaged(s->unsaved));answer("periodicUnsavedDialog",XmDIALOG_CANCEL_BUTTON);assert(!XtAppGetExitFlag(appContext));
    invoke("exitButton");while(XtIsManaged(s->unsaved))answer("periodicUnsavedDialog",XmDIALOG_HELP_BUTTON);assert(XtAppGetExitFlag(appContext));
    printf("PASS Periodic menu actions: %u checks\n",checks);
}
int main(int argc,char **argv){return PeriodicMain(argc,argv);}
