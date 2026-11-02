/* Functional actions for the widget demonstration. All editable examples share
 * explicit document state; no shell command is used for files or clipboard. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "periodic_actions.h"
#include <Xm/XmAll.h>
#include <Xm/Calendar.h>
#include <Xm/SpinB.h>
#include <Xm/Display.h>
#include <Xm/Protocols.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>
#define HISTORY 32
#define FILE_LIMIT (4u*1024u*1024u)
typedef struct Document {
    struct PeriodicActions *owner;
    Widget views[2],last;
    unsigned count;
    char *text,*saved,*path,*undo[HISTORY];
    unsigned undo_count;
    Boolean syncing;
} Document;
typedef enum { FILE_PREVIEW,FILE_OPEN,FILE_SAVE,FILE_INCLUDE,FILE_DRAFT,FILE_MERGE } FilePurpose;
struct PeriodicActions {
    Widget root,main,status,editor,editor_text,query,find_status;
    Widget help,help_text,tutorial_back,tutorial_next;
    Widget preview,preview_text,printer,print_button,print_cancel;
    Widget calendar,calendar_book,calendar_month,calendar_year[12],day_label,week_labels[7];
    Widget file,unsaved,overwrite,arrow;
    Document document,template_document,*editing,*guard;
    Boolean discard_main,discard_template;
    FilePurpose file_purpose;
    int pending,tutorial_step;
    char *pending_path,*print_path;
    Boolean case_sensitive,word_wrap,context_mode,tile,message_preview,arrangement_set;
    pid_t print_pid;int print_fd;XtInputId print_input;
    char print_output[4096];size_t print_used;
};
static void dispatch(PeriodicActions *,Widget,int,XtPointer);
static void run_pending(PeriodicActions *);
static void update_find(PeriodicActions *,Boolean);
static void file_accept(Widget,XtPointer,XtPointer);
static void arrange(PeriodicActions *,Boolean);
static void label(Widget widget,const char *text)
{ XmString value=XmStringCreateLocalized((char *)text);XtVaSetValues(widget,XmNlabelString,value,NULL);XmStringFree(value); }
static void title(Widget dialog,const char *text)
{ XmString value=XmStringCreateLocalized((char *)text);XtVaSetValues(dialog,XmNdialogTitle,value,NULL);XmStringFree(value); }
static void status(PeriodicActions *s,const char *format,...)
{
    char buffer[512];va_list args;va_start(args,format);vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);label(s->status,buffer);
}
static Widget button(Widget parent,const char *name,const char *text,XtCallbackProc callback,XtPointer closure)
{
    Widget item=XtVaCreateManagedWidget(name,xmPushButtonWidgetClass,parent,NULL);label(item,text);
    if(callback)XtAddCallback(item,XmNactivateCallback,callback,closure);return item;
}
static Widget panel(Widget parent,const char *name,Boolean horizontal)
{ return XtVaCreateManagedWidget(name,xmRowColumnWidgetClass,parent,XmNorientation,horizontal?XmHORIZONTAL:XmVERTICAL,XmNpacking,XmPACK_TIGHT,NULL); }
static Widget text_area(Widget parent,const char *name,Boolean editable,int rows,int columns)
{
    /* XmText requires scrollHorizontal=False at creation for wrapping.
     * With wrap off, cursor movement still scrolls a long line horizontally. */
    Arg args[1];XtSetArg(args[0],XmNscrollHorizontal,False);
    Widget text=XmCreateScrolledText(parent,(char *)name,args,1);
    XtVaSetValues(text,XmNeditMode,XmMULTI_LINE_EDIT,XmNeditable,editable,
        XmNcursorPositionVisible,editable,XmNrows,rows,XmNcolumns,columns,
        XmNwordWrap,True,XmNscrollHorizontal,False,NULL);XtManageChild(text);return text;
}
static Widget dialog(PeriodicActions *s,const char *name,const char *caption)
{
    Widget form=XmCreateFormDialog(s->root,(char *)name,NULL,0);title(form,caption);
    Dimension margin=12;XmGetAppearanceSpacing(XtDisplay(s->root),XmAPPEARANCE_SPACING_MEDIUM,&margin);
    XtVaSetValues(form,XmNautoUnmanage,False,XmNmarginWidth,margin,XmNmarginHeight,margin,NULL);return form;
}
static void attach_fill(Widget child,Widget top,Widget bottom)
{
    XtVaSetValues(child,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,
        XmNtopAttachment,top?XmATTACH_WIDGET:XmATTACH_FORM,XmNtopWidget,top,
        XmNbottomAttachment,bottom?XmATTACH_WIDGET:XmATTACH_FORM,XmNbottomWidget,bottom,NULL);
}
static void hide(Widget widget,XtPointer client,XtPointer call)
{ (void)widget;(void)call;XtUnmanageChild((Widget)client); }
static void show(Widget widget)
{ XtManageChild(widget);if(XtIsRealized(XtParent(widget)))XRaiseWindow(XtDisplay(widget),XtWindow(XtParent(widget))); }
static void help_message(PeriodicActions *s,const char *caption,const char *message)
{
    title(s->help,caption);XmTextSetString(s->help_text,(char *)message);
    XtUnmanageChild(s->tutorial_back);XtUnmanageChild(s->tutorial_next);show(s->help);
}
static void error(PeriodicActions *s,const char *operation,const char *detail)
{
    size_t size=strlen(operation)+strlen(detail)+3;char *buffer=XtMalloc(size);snprintf(buffer,size,"%s\n\n%s",operation,detail);help_message(s,"Could not complete action",buffer);XtFree(buffer);status(s,"%s — see the error window.",operation);
}
static Boolean dirty(Document *doc) { return strcmp(doc->text,doc->saved)!=0; }
static void clear_history(Document *doc)
{ for(unsigned i=0;i<doc->undo_count;i++)XtFree(doc->undo[i]);doc->undo_count=0; }
static void remember(Document *doc)
{
    if(doc->undo_count==HISTORY) {XtFree(doc->undo[0]);memmove(doc->undo,doc->undo+1,(HISTORY-1)*sizeof(char *));doc->undo_count--;}
    doc->undo[doc->undo_count++]=XtNewString(doc->text);
}
static void sync_document(Document *doc,Widget origin,const char *text)
{
    doc->syncing=True;
    for(unsigned i=0;i<doc->count;i++)if(doc->views[i]!=origin)XmTextSetString(doc->views[i],(char *)text);
    doc->syncing=False;XtFree(doc->text);doc->text=XtNewString(text);
}
static void document_changed(Widget widget,XtPointer client,XtPointer call)
{
    Document *doc=client;(void)call;if(doc->syncing)return;
    char *text=XmTextGetString(widget);
    if(strcmp(text,doc->text)) { remember(doc);sync_document(doc,widget,text); }
    XtFree(text);
    if(doc==&doc->owner->document)update_find(doc->owner,False);
}
static void focus_text(Widget widget,XtPointer client,XEvent *event,Boolean *cont)
{
    Document *doc=client;(void)cont;if(event->type==FocusIn){doc->last=widget;doc->owner->editing=doc;}
}
static void init_document(PeriodicActions *s,Document *doc,Widget a,Widget b)
{
    doc->owner=s;doc->views[doc->count++]=a;if(b)doc->views[doc->count++]=b;doc->last=a;
    doc->text=XmTextGetString(a);doc->saved=XtNewString(doc->text);
    for(unsigned i=0;i<doc->count;i++) {XtAddCallback(doc->views[i],XmNvalueChangedCallback,document_changed,doc);XtAddEventHandler(doc->views[i],FocusChangeMask,False,focus_text,doc);}
}
static void replace_document(Document *doc,const char *text,const char *path)
{
    clear_history(doc);sync_document(doc,NULL,text);XtFree(doc->saved);doc->saved=XtNewString(text);
    XtFree(doc->path);doc->path=path?XtNewString(path):NULL;
}
static void show_editor(PeriodicActions *s)
{ show(s->editor);s->editing=&s->document;XmProcessTraversal(s->editor_text,XmTRAVERSE_CURRENT); }
static void update_find(PeriodicActions *s,Boolean advance)
{
    wchar_t *text=XmTextGetStringWcs(s->editor_text),*query=XmTextFieldGetStringWcs(s->query);
    size_t length=wcslen(text),n=wcslen(query),matches=0;
    XmTextPosition next=-1,first=-1,start=XmTextGetInsertionPosition(s->editor_text);
    if(n)for(size_t i=0;i+n<=length;i++) {
        size_t j=0;for(;j<n;j++)if((s->case_sensitive?text[i+j]:towlower(text[i+j]))!=(s->case_sensitive?query[j]:towlower(query[j])))break;
        if(j==n){matches++;if(first<0)first=(XmTextPosition)i;if(next<0 && (XmTextPosition)i>=start)next=(XmTextPosition)i;}
    }
    char message[160];snprintf(message,sizeof(message),"%zu match%s — %s",matches,matches==1?"":"es",s->case_sensitive?"case sensitive":"ignore case");label(s->find_status,n?message:"Enter text to find.");
    if(advance && first>=0) {if(next<0)next=first;XmTextSetSelection(s->editor_text,next,next+(XmTextPosition)n,XtLastTimestampProcessed(XtDisplay(s->root)));XmTextSetInsertionPosition(s->editor_text,next+(XmTextPosition)n);XmTextShowPosition(s->editor_text,next);}
    XtFree((char *)text);XtFree((char *)query);
}
static void find_next(Widget w,XtPointer client,XtPointer call)
{(void)w;(void)call;update_find(client,True);}
static void find_changed(Widget w,XtPointer client,XtPointer call)
{(void)w;(void)call;update_find(client,False);}
static void set_wrap(PeriodicActions *s)
{
    for(unsigned d=0;d<2;d++) {Document *doc=d?&s->template_document:&s->document;
        for(unsigned i=0;i<doc->count;i++) {
            Widget view=doc->views[i];
            XtVaSetValues(view,XmNwordWrap,s->word_wrap,XmNresizeWidth,False,NULL);
        }}
}
static void free_document(Document *doc)
{ clear_history(doc);XtFree(doc->text);XtFree(doc->saved);XtFree(doc->path); }
static Boolean valid_text(const char *text,size_t length)
{
    mbstate_t state={0};size_t offset=0;
    while(offset<length) {wchar_t c;size_t n=mbrtowc(&c,text+offset,length-offset,&state);if(!n || n==(size_t)-1 || n==(size_t)-2)return False;offset+=n;}
    return True;
}
static char *read_file(PeriodicActions *s,const char *path,Boolean allow_binary,Boolean *binary)
{
    struct stat st;*binary=False;
    int fd=open(path,O_RDONLY);
    if(fd<0){error(s,"Cannot open file",strerror(errno));return NULL;}
    if(fstat(fd,&st)<0 || !S_ISREG(st.st_mode) || st.st_size<0 || (uintmax_t)st.st_size>FILE_LIMIT) {
        close(fd);error(s,"Cannot open file","Choose a regular file no larger than 4 MiB.");return NULL;
    }
    size_t length=(size_t)st.st_size,done=0;char *text=XtMalloc(length+1);
    while(done<length) {ssize_t n=read(fd,text+done,length-done);if(n<0 && errno==EINTR)continue;if(n<=0){close(fd);XtFree(text);error(s,"Cannot read file",n?strerror(errno):"The file changed while it was being read.");return NULL;}done+=(size_t)n;}
    close(fd);text[length]=0;
    if(!valid_text(text,length)) {
        if(!allow_binary){XtFree(text);error(s,"Cannot load text","The file is not valid text in the current locale, or contains NUL bytes.");return NULL;}
        *binary=True;char message[256];snprintf(message,sizeof(message),"Binary file selected\n\nSize: %zu bytes\n\nUse the Text filter to select an editable text document.",length);XtFree(text);return XtNewString(message);
    }
    return text;
}
static Boolean write_file(PeriodicActions *s,Document *doc,const char *path)
{
    struct stat st;Boolean exists=stat(path,&st)==0;
    if(exists && access(path,W_OK)<0){error(s,"Cannot save read-only file",strerror(errno));return False;}
    if(exists && !S_ISREG(st.st_mode)){error(s,"Cannot save file","The destination is not a regular file.");return False;}
    char *resolved=exists?realpath(path,NULL):NULL;const char *destination=resolved?resolved:path;
    const char *slash=strrchr(destination,'/');size_t dir=slash?(size_t)(slash-destination)+1:0;
    char *temporary=XtMalloc(dir+32);
    if(dir)memcpy(temporary,destination,dir);else {temporary[0]='.';temporary[1]='/';dir=2;}
    strcpy(temporary+dir,".periodic-XXXXXX");int fd=mkstemp(temporary),failure=0;
    if(fd<0)failure=errno;
    if(!failure && exists && fchmod(fd,st.st_mode&0777)<0)failure=errno;
    size_t length=strlen(doc->text),done=0;
    while(!failure && done<length) {ssize_t n=write(fd,doc->text+done,length-done);if(n<0 && errno==EINTR)continue;if(n<=0){failure=n?errno:EIO;break;}done+=(size_t)n;}
    if(!failure && fsync(fd)<0)failure=errno;
    if(fd>=0 && close(fd)<0 && !failure)failure=errno;
    if(!failure && rename(temporary,destination)<0)failure=errno;
    if(failure){unlink(temporary);error(s,"Cannot save file",strerror(failure));}
    else {char *saved_path=XtNewString(path);XtFree(doc->path);doc->path=saved_path;XtFree(doc->saved);doc->saved=XtNewString(doc->text);status(s,"Saved document (%zu bytes).",length);}
    XtFree(temporary);free(resolved);return failure==0;
}
static Document *file_document(PeriodicActions *s)
{ return s->file_purpose==FILE_DRAFT?&s->template_document:&s->document; }
static void choose_file(PeriodicActions *s,FilePurpose purpose)
{
    s->file_purpose=purpose;
    Widget merge=XtNameToWidget(s->file,"*fileMergeButton");if(merge)XtSetSensitive(merge,purpose==FILE_PREVIEW || purpose==FILE_OPEN);
    const char *caption=purpose==FILE_SAVE?"Save document as":purpose==FILE_DRAFT?"Save message draft":purpose==FILE_INCLUDE?"Include text in template":purpose==FILE_MERGE?"Merge text into document":purpose==FILE_OPEN?"Open text document":"Preview selected file";
    title(s->file,caption);label(XmFileSelectionBoxGetChild(s->file,XmDIALOG_OK_BUTTON),
        purpose==FILE_SAVE || purpose==FILE_DRAFT?"Save":purpose==FILE_INCLUDE || purpose==FILE_MERGE?"Include":"Open");
    show(s->file);
}
static void file_cancel(Widget w,XtPointer client,XtPointer call)
{
    PeriodicActions *s=client;(void)w;(void)call;XtUnmanageChild(s->file);s->pending=0;s->file_purpose=FILE_PREVIEW;status(s,"File operation cancelled; document retained.");
}
static void overwrite_yes(Widget w,XtPointer client,XtPointer call)
{
    PeriodicActions *s=client;(void)w;(void)call;XtUnmanageChild(s->overwrite);
    if(write_file(s,file_document(s),s->pending_path)) {XtUnmanageChild(s->file);s->file_purpose=FILE_PREVIEW;run_pending(s);}
    XtFree(s->pending_path);s->pending_path=NULL;
}
static void overwrite_no(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)w;(void)call;XtUnmanageChild(s->overwrite);XtFree(s->pending_path);s->pending_path=NULL;show(s->file);status(s,"Overwrite cancelled; existing file retained.");}
static void file_accept(Widget widget,XtPointer client,XtPointer call)
{
    PeriodicActions *s=client;XmFileSelectionBoxCallbackStruct *cb=call;char *path=NULL;(void)widget;
    if(!cb || !cb->value || !XmStringGetLtoR(cb->value,XmFONTLIST_DEFAULT_TAG,&path) || !*path) {XtFree(path);error(s,"No filename","Select or enter a filename first.");return;}
    if(s->file_purpose==FILE_SAVE || s->file_purpose==FILE_DRAFT) {
        struct stat st;
        if(stat(path,&st)==0) {XtFree(s->pending_path);s->pending_path=XtNewString(path);show(s->overwrite);XtFree(path);return;}
        if(write_file(s,file_document(s),path)) {XtUnmanageChild(s->file);s->file_purpose=FILE_PREVIEW;run_pending(s);}
    } else {
        Boolean binary;char *text=read_file(s,path,s->file_purpose==FILE_PREVIEW,&binary);
        if(text) {
            if(binary)help_message(s,"Selected file",text);
            else if(s->file_purpose==FILE_INCLUDE || s->file_purpose==FILE_MERGE) {
                Document *doc=s->file_purpose==FILE_INCLUDE?&s->template_document:&s->document;
                Widget target=doc->last;XmTextInsert(target,XmTextGetInsertionPosition(target),text);
                if(doc==&s->document)show_editor(s);status(s,"Included selected text.");
            } else {replace_document(&s->document,text,path);show_editor(s);status(s,"Opened text document (%zu bytes).",strlen(text));}
            XtFree(text);XtUnmanageChild(s->file);s->file_purpose=FILE_PREVIEW;
        }
    }
    XtFree(path);
}
static void save_document(PeriodicActions *s,Document *doc,Boolean as)
{
    if(as || !doc->path)choose_file(s,doc==&s->template_document?FILE_DRAFT:FILE_SAVE);
    else if(write_file(s,doc,doc->path))run_pending(s);
}
static void unsaved_save(Widget w,XtPointer client,XtPointer call);
static void unsaved_discard(Widget w,XtPointer client,XtPointer call);
static void unsaved_cancel(Widget w,XtPointer client,XtPointer call);
static void request(PeriodicActions *s,int action)
{ s->pending=action;s->discard_main=s->discard_template=False;run_pending(s); }
static void run_pending(PeriodicActions *s)
{
    if(!s->pending)return;
    Document *guard=dirty(&s->document) && !s->discard_main?&s->document:
        s->pending==PA_EXIT && dirty(&s->template_document) && !s->discard_template?&s->template_document:NULL;
    if(guard) {s->guard=guard;title(s->unsaved,guard==&s->document?"Unsaved document":"Unsaved message draft");show(s->unsaved);return;}
    int action=s->pending;s->pending=0;s->discard_main=s->discard_template=False;
    if(action==PA_NEW){replace_document(&s->document,"",NULL);show_editor(s);status(s,"New document.");}
    else if(action==PA_OPEN)choose_file(s,FILE_OPEN);
    else if(action==PA_EXIT) {
        if(s->print_pid>0)help_message(s,"Print submission active","Wait for the submission to finish, or cancel it in the Print window, then choose Exit again.");
        else XtAppSetExitFlag(XtWidgetToApplicationContext(s->root));
    }
}
static void unsaved_save(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)w;(void)call;XtUnmanageChild(s->unsaved);save_document(s,s->guard,False);}
static void unsaved_discard(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)w;(void)call;XtUnmanageChild(s->unsaved);if(s->guard==&s->document)s->discard_main=True;else s->discard_template=True;run_pending(s);}
static void unsaved_cancel(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)w;(void)call;XtUnmanageChild(s->unsaved);s->pending=0;s->discard_main=s->discard_template=False;status(s,"Cancelled; unsaved text retained.");}
static void edit(PeriodicActions *s,int action)
{
    Document *doc=s->editing?s->editing:&s->document;Widget target=doc->last;Boolean done=True;
    Time time=XtLastTimestampProcessed(XtDisplay(s->root));
    if(action==PA_UNDO) {
        if(doc->undo_count) {char *previous=doc->undo[--doc->undo_count];sync_document(doc,NULL,previous);XtFree(previous);}
        else done=False;
    } else if(action==PA_COPY)done=XmTextCopy(target,time);
    else if(action==PA_CUT)done=XmTextCut(target,time);
    else if(action==PA_PASTE)done=XmTextPaste(target);
    else if(action==PA_CLEAR)XmTextSetString(target,"");
    else if(action==PA_DELETE)done=XmTextRemove(target);
    if(doc==&s->document)show_editor(s);
    status(s,done?"Edit action applied to the active text example.":action==PA_UNDO?"Nothing to undo.":action==PA_PASTE?"No text is available on the clipboard.":"Select text in an editor first.");
}
static void filter_files(PeriodicActions *s,const char *pattern)
{
    XmString directory=NULL;char *path=NULL;XtVaGetValues(s->file,XmNdirectory,&directory,NULL);
    if(!XmStringGetLtoR(directory,XmFONTLIST_DEFAULT_TAG,&path))path=XtNewString("./");
    size_t size=strlen(path)+strlen(pattern)+2;char *mask=XtMalloc(size);snprintf(mask,size,"%s%s%s",path,path[0] && path[strlen(path)-1]=='/'?"":"/",pattern);
    XmString search=XmStringCreateLocalized(mask);XmFileSelectionDoSearch(s->file,search);XmStringFree(search);XtFree(mask);XtFree(path);XmStringFree(directory);status(s,"File filter: %s",pattern);
}
static const char *sample_dialogs[]={"*fileDialog","*selectDialog","*templateDialog","*infoDialog","*warningDialog","*errorDialog","*questionDialog","*workingDialog","*promptDialog","*showDialog"};
static void arrange(PeriodicActions *s,Boolean report)
{
    if(!s->arrangement_set && !report)return;
    Widget dialogs[XtNumber(sample_dialogs)+4];unsigned count=0;
    for(unsigned i=0;i<XtNumber(sample_dialogs);i++){Widget item=XtNameToWidget(s->root,(char *)sample_dialogs[i]);if(item && XtIsManaged(item))dialogs[count++]=item;}
    Widget extra[]={s->editor,s->help,s->preview,s->calendar};
    for(unsigned i=0;i<XtNumber(extra);i++)if(extra[i] && XtIsManaged(extra[i]))dialogs[count++]=extra[i];
    int screen_width=WidthOfScreen(XtScreen(s->root)),screen_height=HeightOfScreen(XtScreen(s->root));
    unsigned columns=1;while(columns*columns<count)columns++;
    unsigned rows=count?(count+columns-1)/columns:1;
    for(unsigned i=0;i<count;i++) {
        Widget window=XtParent(dialogs[i]);Dimension width,height;XtVaGetValues(window,XtNwidth,&width,XtNheight,&height,NULL);
        int x=s->tile?(int)((i%columns)*screen_width/columns):40+(int)i*28;
        int y=s->tile?40+(int)((i/columns)*(screen_height-80)/rows):60+(int)i*28;
        if(x+(int)width>screen_width)x=screen_width>width?screen_width-width:0;
        if(y+(int)height>screen_height)y=screen_height>height?screen_height-height:0;
        XtVaSetValues(dialogs[i],XmNdefaultPosition,False,NULL);XtVaSetValues(window,XtNx,x,XtNy,y,NULL);
        if(XtIsRealized(window))XRaiseWindow(XtDisplay(window),XtWindow(window));
    }
    if(report)status(s,"%s: %u open example window%s. Open more from Notebook > Dialogs.",s->tile?"Tile":"Stack",count,count==1?"":"s");
}
static void select_radio(PeriodicActions *s,const char *const *names,unsigned count,unsigned selected)
{ for(unsigned i=0;i<count;i++){Widget w=XtNameToWidget(s->root,(char *)names[i]);if(w)XmToggleButtonSetState(w,i==selected,False);} }
static void calendar_view(PeriodicActions *s,unsigned mode)
{
    static const char *names[]={"*dayButton","*weekButton","*monthButton","*yearButton"};
    static const char *captions[]={"Day view","Week view","Month view","Year view"};
    select_radio(s,names,4,mode);time_t now=time(NULL);struct tm date;localtime_r(&now,&date);
    char message[160];strftime(message,sizeof(message),"%A, %d %B %Y",&date);label(s->day_label,message);
    struct tm monday=date;monday.tm_mday-=(date.tm_wday+6)%7;monday.tm_hour=12;monday.tm_isdst=-1;mktime(&monday);
    for(unsigned i=0;i<7;i++){struct tm day=monday;day.tm_mday+=(int)i;day.tm_isdst=-1;mktime(&day);strftime(message,sizeof(message),"%A\n%d %B %Y",&day);label(s->week_labels[i],message);}
    XmCalendarDate today={date.tm_year+1900,date.tm_mon+1,date.tm_mday};
    XmCalendarSetDate(s->calendar_month,&today,False);XmCalendarSetVisibleMonth(s->calendar_month,today.year,today.month);
    for(unsigned i=0;i<12;i++){XmCalendarSetDate(s->calendar_year[i],&today,False);XmCalendarSetVisibleMonth(s->calendar_year[i],today.year,(int)i+1);}
    XtVaSetValues(s->calendar_book,XmNcurrentPageNumber,mode+1,NULL);title(s->calendar,captions[mode]);show(s->calendar);status(s,"Calendar: %s.",captions[mode]);
}
static const char *context_description(Widget w)
{
    if(XmIsArrowButton(w) || XmIsArrowButtonGadget(w))return "ArrowButton: click or focus an arrow, then Options > Direction changes that arrow. The rounded outline shows keyboard focus. Space activates it.";
    if(XmIsText(w) || XmIsTextField(w))return "Text: type and select text. File and Edit act on the document; Options > Case Sensitive controls Find, and Word Wrap controls line wrapping. Use the document window for a larger editor.";
    if(XmIsToggleButton(w) || XmIsToggleButtonGadget(w))return "ToggleButton: click or press Space to change its checked state. The Lights example changes the DrawnButton artwork.";
    if(XmIsScale(w))return "Scale: drag the thumb or use arrow keys to change the radio frequency. The RadioBox presets change this same value.";
    if(XmIsContainer(w) || XmIsIconGadget(w))return "Container: select an item, then double-click or activate its default action to open the corresponding dialog or information. Font changes also reflow spatial icon cells.";
    if(XmIsList(w))return "List: click to select, use arrows to navigate and type to locate entries. Selections and list contents survive font changes.";
    if(XmIsComboBox(w))return "ComboBox: type text or open the arrow to select an item from its list.";
    if(XtIsSubclass(w,xmSpinBoxWidgetClass))return "SpinBox: use the arrow controls to change the value, or edit its text field.";
    if(XmIsCalendar(w))return "Calendar: select a date and use the header arrows to browse months. View > Day, Week, Month and Year choose different previews of the current civil date.";
    if(XmIsNotebook(w))return "Notebook: choose a tab to explore Dialogs, Widgets, Layouts or Toggle controls.";
    if(XmIsPushButton(w) || XmIsPushButtonGadget(w))return "PushButton: hold it to see pressed feedback, then release to activate. Space activates a focused button. Start Filming changes to Stop Filming as a reversible demonstration.";
    return "Motif example: use Tab to move keyboard focus, Space or Enter to activate, and the mouse to explore. Help > Tutorial explains the main examples and menus.";
}
static Widget object_at(Widget parent,int x,int y)
{
    if(XtIsComposite(parent)){WidgetList children;Cardinal count;XtVaGetValues(parent,XtNchildren,&children,XtNnumChildren,&count,NULL);
        for(Cardinal i=0;i<count;i++)if(XtIsManaged(children[i])){Position cx,cy;Dimension width,height;XtVaGetValues(children[i],XmNx,&cx,XmNy,&cy,XmNwidth,&width,XmNheight,&height,NULL);
            if(x>=cx && y>=cy && x<cx+width && y<cy+height)return children[i];}}
    return parent;
}
static void context_event(Widget widget,XtPointer client,XEvent *event,Boolean *cont)
{
    PeriodicActions *s=client;
    if(event->type==FocusIn && XmIsArrowButton(widget))s->arrow=widget;
    if(!s->context_mode)return;
    if(event->type==KeyPress && XLookupKeysym(&event->xkey,0)==XK_Escape){s->context_mode=False;*cont=False;status(s,"Context help cancelled.");}
    else if(event->type==ButtonPress && event->xbutton.button==Button1){Widget target=object_at(widget,event->xbutton.x,event->xbutton.y);s->context_mode=False;*cont=False;
        char caption[256];snprintf(caption,sizeof(caption),"Help for %s",XtName(target));help_message(s,caption,context_description(target));status(s,"Context help: %s.",XtName(target));}
}
static void context_watch(PeriodicActions *s,Widget widget)
{
    if(XtIsWidget(widget))XtAddEventHandler(widget,ButtonPressMask|KeyPressMask|FocusChangeMask,False,context_event,s);
    if(XtIsComposite(widget)){WidgetList children;Cardinal count;XtVaGetValues(widget,XtNchildren,&children,XtNnumChildren,&count,NULL);for(Cardinal i=0;i<count;i++)context_watch(s,children[i]);}
}
static const char *tutorial[]={
    "1. Explore widgets\n\nUse Tab and Space, or click. Start Filming changes its label. Lights changes the DrawnButton, and the radio presets update the Scale. Notebook tabs expose more examples.",
    "2. Work with text\n\nFile > New opens the document editor. Type or open a UTF-8 text file. Edit supplies Undo, Cut, Copy, Paste, Clear and Delete. The overview Text example mirrors the same document.",
    "3. Find and wrap\n\nType in Find and choose Find next. Options > Case Sensitive controls matching; Word Wrap changes editor wrapping. Neither setting changes your text.",
    "4. Change presentation\n\nOptions > Font changes the application text size. Focus an arrow, then Options > Direction changes it. View > Day, Week, Month or Year opens the corresponding calendar preview.",
    "5. Explore windows and files\n\nOpen several Notebook > Dialogs samples. View > Stack and Tile arrange them. File-filter choices narrow the sample file list. Template Include, Draft and Spell operate on the template text. Help > On Context lets you click a widget for guidance; Escape cancels."
};
static void tutorial_update(PeriodicActions *s)
{
    title(s->help,"Periodic tutorial");XmTextSetString(s->help_text,(char *)tutorial[s->tutorial_step]);XtManageChild(s->tutorial_back);XtManageChild(s->tutorial_next);
    XtSetSensitive(s->tutorial_back,s->tutorial_step>0);XtSetSensitive(s->tutorial_next,s->tutorial_step+1<(int)XtNumber(tutorial));show(s->help);
}
static void tutorial_back(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)w;(void)call;if(s->tutorial_step>0)s->tutorial_step--;tutorial_update(s);}
static void tutorial_next(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)w;(void)call;if(s->tutorial_step+1<(int)XtNumber(tutorial))s->tutorial_step++;tutorial_update(s);}
static int compare_words(const void *a,const void *b){return strcasecmp(*(const char *const *)a,*(const char *const *)b);}
static void spell(PeriodicActions *s)
{
    const char *configured=getenv("PERIODIC_DICTIONARY");const char *paths[]={configured,"/usr/share/dict/words","/usr/share/dict/american-english"};FILE *file=NULL;
    for(unsigned i=0;i<XtNumber(paths);i++)if(paths[i] && (file=fopen(paths[i],"r")))break;
    if(!file){error(s,"English dictionary unavailable","Install a system English word list, or set PERIODIC_DICTIONARY to a UTF-8 word list (one word per line).");return;}
    char **words=NULL,*line=NULL;size_t capacity=0,count=0,allocated=0;ssize_t length;
    while((length=getline(&line,&capacity,file))>=0){while(length && (line[length-1]=='\r' || line[length-1]=='\n'))line[--length]=0;if(!length)continue;
        if(count==allocated){allocated=allocated?allocated*2:4096;words=(char **)XtRealloc((char *)words,allocated*sizeof(char *));}words[count++]=XtNewString(line);}
    free(line);fclose(file);qsort(words,count,sizeof(char *),compare_words);
    char output[8192]="English spelling check\n\nUnknown words:\n",token[256];size_t used=strlen(output),unknown=0;char *text=XmTextGetString(s->template_document.last);
    for(size_t i=0;text[i];){while(text[i] && !((text[i]>='A' && text[i]<='Z') || (text[i]>='a' && text[i]<='z') || (unsigned char)text[i]>=128))i++;
        size_t n=0;Boolean non_ascii=False;while(text[i] && (((text[i]>='A' && text[i]<='Z') || (text[i]>='a' && text[i]<='z')) || text[i]=='\'' || (unsigned char)text[i]>=128)) {if((unsigned char)text[i]>=128)non_ascii=True;if(n+1<sizeof(token))token[n++]=text[i];i++;}token[n]=0;
        if(n && !non_ascii){char *key=token;if(!bsearch(&key,words,count,sizeof(char *),compare_words)){unknown++;if(used+n+2<sizeof(output)){memcpy(output+used,token,n);used+=n;output[used++]='\n';output[used]=0;}}}}
    if(!unknown)snprintf(output,sizeof(output),"English spelling check\n\nNo unknown English words found.\nNon-English tokens are skipped.");
    for(size_t i=0;i<count;i++)XtFree(words[i]);XtFree((char *)words);XtFree(text);help_message(s,"Template spelling",output);status(s,"Spelling: %zu unknown English word%s.",unknown,unknown==1?"":"s");
}
static void print_ready(XtPointer client,int *source,XtInputId *id)
{
    PeriodicActions *s=client;char chunk[1024];ssize_t n=read(*source,chunk,sizeof(chunk));
    if(n<0 && errno==EINTR)return;
    if(n>0){size_t take=(size_t)n;if(take>sizeof(s->print_output)-s->print_used-1)take=sizeof(s->print_output)-s->print_used-1;
        memcpy(s->print_output+s->print_used,chunk,take);s->print_used+=take;s->print_output[s->print_used]=0;return;}
    XtRemoveInput(*id);s->print_input=0;close(s->print_fd);s->print_fd=-1;
    int result=0;while(waitpid(s->print_pid,&result,0)<0 && errno==EINTR){}s->print_pid=0;
    unlink(s->print_path);XtFree(s->print_path);s->print_path=NULL;
    XtSetSensitive(s->print_button,True);XtSetSensitive(s->print_cancel,False);
    if(WIFEXITED(result) && WEXITSTATUS(result)==0){help_message(s,"Print submitted",s->print_output[0]?s->print_output:"The print service accepted the document.");status(s,"Print service accepted the document.");}
    else error(s,"Print submission failed",s->print_output[0]?s->print_output:"Check that a CUPS-compatible lp program and printer are configured.");
}
static void print_document(Widget widget,XtPointer client,XtPointer call)
{
    PeriodicActions *s=client;(void)widget;(void)call;if(s->print_pid>0)return;
    const char *tmp=getenv("TMPDIR");if(!tmp || !*tmp)tmp=P_tmpdir;
    char *path=XtMalloc(strlen(tmp)+40);sprintf(path,"%s%speriodic-print-XXXXXX",tmp,tmp[strlen(tmp)-1]=='/'?"":"/");
    int fd=mkstemp(path),pipefd[2];char *text=XmTextGetString(s->preview_text),*printer=XmTextFieldGetString(s->printer);
    if(fd<0){error(s,"Cannot prepare printing",strerror(errno));XtFree(path);XtFree(text);XtFree(printer);return;}
    size_t length=strlen(text),done=0;int failure=0;
    while(done<length){ssize_t n=write(fd,text+done,length-done);if(n<0 && errno==EINTR)continue;if(n<=0){failure=errno?errno:EIO;break;}done+=(size_t)n;}
    if(close(fd)<0 && !failure)failure=errno;
    if(failure || pipe(pipefd)<0){error(s,"Cannot prepare printing",strerror(failure?failure:errno));unlink(path);XtFree(path);XtFree(text);XtFree(printer);return;}
    pid_t pid=fork();
    if(pid==0){dup2(pipefd[1],STDOUT_FILENO);dup2(pipefd[1],STDERR_FILENO);close(pipefd[0]);close(pipefd[1]);
        if(*printer)execlp("lp","lp","-d",printer,"-t","Motif demo document",path,(char *)NULL);
        else execlp("lp","lp","-t","Motif demo document",path,(char *)NULL);
        dprintf(STDERR_FILENO,"Cannot run lp: %s\n",strerror(errno));_exit(127);}
    close(pipefd[1]);XtFree(text);XtFree(printer);
    if(pid<0){close(pipefd[0]);unlink(path);XtFree(path);error(s,"Cannot start printing",strerror(errno));return;}
    s->print_pid=pid;s->print_fd=pipefd[0];s->print_path=path;s->print_used=0;s->print_output[0]=0;
    s->print_input=XtAppAddInput(XtWidgetToApplicationContext(s->root),s->print_fd,(XtPointer)XtInputReadMask,print_ready,s);
    XtSetSensitive(s->print_button,False);XtSetSensitive(s->print_cancel,True);status(s,"Submitting document to the print service…");
}
static void cancel_print(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)w;(void)call;if(s->print_pid>0){kill(s->print_pid,SIGTERM);status(s,"Cancelling submission. Check the printer queue if it was already accepted.");}}
static void preview_draft(Widget w,XtPointer client,XtPointer call)
{(void)w;(void)call;save_document(client,&((PeriodicActions *)client)->template_document,True);}
static void preview_document(PeriodicActions *s,Boolean message)
{
    s->message_preview=message;title(s->preview,message?"Local message preview":"Print document");
    XmTextSetString(s->preview_text,message?s->template_document.text:s->document.text);
    Widget print_bar=XtParent(s->printer);if(message)XtUnmanageChild(print_bar);else XtManageChild(print_bar);
    attach_fill(XtParent(s->preview_text),message?NULL:print_bar,XtNameToWidget(s->preview,"*previewButtons"));
    Widget draft=XtNameToWidget(s->preview,"*savePreviewDraft");if(message)XtManageChild(draft);else XtUnmanageChild(draft);
    show(s->preview);status(s,message?"Message preview — no mail is sent. Save Draft writes it to a file.":"Print preview: choose a printer, then Print to submit.");
}
static void main_close(Widget w,XtPointer client,XtPointer call)
{(void)w;(void)call;request(client,PA_EXIT);}
static void arrow_used(Widget w,XtPointer client,XtPointer call)
{
    PeriodicActions *s=client;unsigned char direction;(void)call;s->arrow=w;
    XtVaGetValues(w,XmNarrowDirection,&direction,NULL);status(s,"Arrow activated (direction %s).",direction==XmARROW_UP?"Up":direction==XmARROW_DOWN?"Down":direction==XmARROW_LEFT?"Left":"Right");
}
static void destroy_actions(Widget w,XtPointer client,XtPointer call)
{
    PeriodicActions *s=client;(void)w;(void)call;
    if(s->print_input)XtRemoveInput(s->print_input);
    if(s->print_fd>=0)close(s->print_fd);
    if(s->print_pid>0){kill(s->print_pid,SIGKILL);while(waitpid(s->print_pid,NULL,0)<0 && errno==EINTR){}}
    if(s->print_path)unlink(s->print_path);XtFree(s->print_path);XtFree(s->pending_path);
    free_document(&s->document);free_document(&s->template_document);XtFree((char *)s);
}
static void dispatch(PeriodicActions *s,Widget source,int action,XtPointer call)
{
    (void)call;
    if(action>=PA_UNDO && action<=PA_DELETE){edit(s,action);return;}
    if(action>=PA_DAY && action<=PA_YEAR){calendar_view(s,(unsigned)(action-PA_DAY));return;}
    if(action>=PA_UP && action<=PA_RIGHT){static const unsigned char directions[]={XmARROW_UP,XmARROW_DOWN,XmARROW_LEFT,XmARROW_RIGHT};static const char *names[]={"Up","Down","Left","Right"};
        if(s->arrow){XtVaSetValues(s->arrow,XmNarrowDirection,directions[action-PA_UP],NULL);XmProcessTraversal(s->arrow,XmTRAVERSE_CURRENT);status(s,"Selected arrow now points %s.",names[action-PA_UP]);}return;}
    switch(action){
    case PA_NEW:case PA_OPEN:case PA_EXIT:request(s,action);break;
    case PA_SAVE:save_document(s,&s->document,False);break;
    case PA_SAVE_AS:save_document(s,&s->document,True);break;
    case PA_PRINT:preview_document(s,False);break;
    case PA_STACK:case PA_TILE:{static const char *names[]={"*stackButton","*tileButton"};s->tile=action==PA_TILE;s->arrangement_set=True;select_radio(s,names,2,s->tile);arrange(s,True);break;}
    case PA_CASE:s->case_sensitive=XmToggleButtonGetState(source);show_editor(s);update_find(s,True);status(s,"Find is now %s.",s->case_sensitive?"case sensitive":"case insensitive");break;
    case PA_WRAP:s->word_wrap=XmToggleButtonGetState(source);set_wrap(s);show_editor(s);status(s,"Word wrap %s; text retained.",s->word_wrap?"enabled":"disabled");break;
    case PA_CONTEXT:s->context_mode=True;status(s,"Context help: click an example with Button 1; Escape cancels.");break;
    case PA_WINDOW_HELP:help_message(s,"Periodic window help","The Periodic Table demonstrates Motif widgets.\n\nFile and Edit operate on a shared text document. Options controls font size, the last focused arrow, search matching and word wrap. View arranges open sample dialogs or opens calendar previews.\n\nNotebook tabs show dialogs, widgets, layouts and toggle controls. Double-click a dialog icon to open it. Several sample windows can stay open for Stack and Tile.\n\nHelp > On Context lets you click an example for focused guidance. No mail is sent by the template preview.");break;
    case PA_TUTORIAL:s->tutorial_step=0;tutorial_update(s);break;
    case PA_VERSION:{char version[512];snprintf(version,sizeof(version),"%s\nToolkit ABI: %u\n\nPeriodic widget demonstration\nFonts: %s",XmVERSION_STRING,XmToolkitABI(),
#ifdef USE_XFT
        "Xft / Fontconfig"
#else
        "X11 core fonts"
#endif
        );help_message(s,"About Periodic",version);break;}
    case PA_PREVIEW:preview_document(s,True);break;
    case PA_INCLUDE:choose_file(s,FILE_INCLUDE);break;
    case PA_DRAFT:save_document(s,&s->template_document,False);break;
    case PA_SPELL:spell(s);break;
    case PA_TEMPLATE_CLOSE:XtUnmanageChild(XtNameToWidget(s->root,"*templateDialog"));status(s,"Template hidden; its text is retained.");break;
    case PA_AUDIO:filter_files(s,"*.wav");break;
    case PA_GRAPHICS:filter_files(s,"*.png");break;
    case PA_TEXT:filter_files(s,"*.txt");break;
    case PA_FILE_MERGE:{XmString value=NULL;XmFileSelectionBoxCallbackStruct selected={0};FilePurpose previous=s->file_purpose;
        XtVaGetValues(s->file,XmNdirSpec,&value,NULL);selected.value=value;s->file_purpose=FILE_MERGE;file_accept(s->file,s,&selected);if(XtIsManaged(s->file))s->file_purpose=previous;XmStringFree(value);break;}
    case PA_FILE_CLEAR:{XmString empty=XmStringCreateLocalized("");XtVaSetValues(s->file,XmNtextString,empty,NULL);XmStringFree(empty);status(s,"Filename selection cleared.");break;}
    default:error(s,"Unknown action","The menu action has no registered implementation.");break;
    }
}
void PeriodicDispatchAction(PeriodicActions *s,Widget source,int action,XtPointer call)
{if(s)dispatch(s,source,action,call);}
void PeriodicPrepareSampleDialog(PeriodicActions *s,Widget item)
{if(!s)return;if(item==s->file){s->file_purpose=FILE_PREVIEW;title(s->file,"Preview selected file");label(XmFileSelectionBoxGetChild(s->file,XmDIALOG_OK_BUTTON),"Open / Preview");}arrange(s,False);}
static Widget bottom_bar(Widget parent,const char *name)
{
    Widget bar=panel(parent,name,True);XtVaSetValues(bar,XmNleftAttachment,XmATTACH_FORM,
        XmNrightAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,NULL);return bar;
}
static void fit_dialog(PeriodicActions *s,Widget form,unsigned width,unsigned height)
{
    double scale=1.0;XmGetWidgetScale(s->root,&scale);
    unsigned available_width=WidthOfScreen(XtScreen(s->root)),available_height=HeightOfScreen(XtScreen(s->root));
    if(available_width>64)available_width-=64;if(available_height>96)available_height-=96;
    unsigned w=(unsigned)(width*scale+0.5),h=(unsigned)(height*scale+0.5);
    XtVaSetValues(form,XmNwidth,(Dimension)(w<available_width?w:available_width),XmNheight,(Dimension)(h<available_height?h:available_height),NULL);
}
static void create_editor(PeriodicActions *s)
{
    s->editor=dialog(s,"periodicDocumentDialog","Demo document");
    Widget footer=bottom_bar(s->editor,"documentButtons");button(footer,"closeDocument","Close",hide,s->editor);
    Widget search=panel(s->editor,"documentSearch",True);XtVaSetValues(search,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,XmNtopAttachment,XmATTACH_FORM,NULL);
    Widget caption=XtVaCreateManagedWidget("findLabel",xmLabelWidgetClass,search,NULL);label(caption,"Find:");
    s->query=XtVaCreateManagedWidget("findQuery",xmTextFieldWidgetClass,search,XmNcolumns,20,NULL);XmTextFieldSetString(s->query,"motif");
    button(search,"findNext","Find next",find_next,s);s->find_status=XtVaCreateManagedWidget("findStatus",xmLabelWidgetClass,search,NULL);
    XtAddCallback(s->query,XmNvalueChangedCallback,find_changed,s);XtAddCallback(s->query,XmNactivateCallback,find_next,s);
    s->editor_text=text_area(s->editor,"documentText",True,18,72);attach_fill(XtParent(s->editor_text),search,footer);fit_dialog(s,s->editor,860,500);
}
static void create_help(PeriodicActions *s)
{
    s->help=dialog(s,"periodicHelpDialog","Periodic help");Widget bar=bottom_bar(s->help,"helpButtons");
    s->tutorial_back=button(bar,"tutorialBack","Back",tutorial_back,s);s->tutorial_next=button(bar,"tutorialNext","Next",tutorial_next,s);
    button(bar,"closeHelp","Close",hide,s->help);XtUnmanageChild(s->tutorial_back);XtUnmanageChild(s->tutorial_next);
    s->help_text=text_area(s->help,"helpText",False,18,72);attach_fill(XtParent(s->help_text),NULL,bar);fit_dialog(s,s->help,800,430);
}
static void create_preview(PeriodicActions *s)
{
    s->preview=dialog(s,"periodicPreviewDialog","Print document");Widget bar=bottom_bar(s->preview,"previewButtons");
    button(bar,"closePreview","Close",hide,s->preview);button(bar,"savePreviewDraft","Save Draft…",preview_draft,s);
    Widget options=panel(s->preview,"printOptions",True);XtVaSetValues(options,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,XmNtopAttachment,XmATTACH_FORM,NULL);
    Widget caption=XtVaCreateManagedWidget("printerLabel",xmLabelWidgetClass,options,NULL);label(caption,"Printer (empty = default):");
    s->printer=XtVaCreateManagedWidget("printerName",xmTextFieldWidgetClass,options,XmNcolumns,18,NULL);
    s->print_button=button(options,"submitPrint","Print",print_document,s);s->print_cancel=button(options,"cancelPrint","Cancel submission",cancel_print,s);XtSetSensitive(s->print_cancel,False);
    s->preview_text=text_area(s->preview,"previewText",False,20,72);attach_fill(XtParent(s->preview_text),options,bar);fit_dialog(s,s->preview,860,500);
}
static Widget calendar_page(Widget book,const char *name,unsigned page)
{
    Widget panel=XtVaCreateManagedWidget(name,xmRowColumnWidgetClass,book,XmNnotebookChildType,XmPAGE,XmNpageNumber,page,XmNorientation,XmVERTICAL,NULL);return panel;
}
static void calendar_changed(Widget widget,XtPointer client,XtPointer call)
{
    PeriodicActions *s=client;XmNotebookCallbackStruct *page=call;(void)widget;
    static const char *names[]={"*dayButton","*weekButton","*monthButton","*yearButton"};
    if(page && page->page_number>=1 && page->page_number<=4)
        select_radio(s,names,4,(unsigned)page->page_number-1);
}
static void create_calendar(PeriodicActions *s)
{
    s->calendar=dialog(s,"periodicCalendarDialog","Calendar previews");Widget bar=bottom_bar(s->calendar,"calendarButtons");button(bar,"closeCalendar","Close",hide,s->calendar);
    s->calendar_book=XtVaCreateManagedWidget("calendarViews",xmNotebookWidgetClass,s->calendar,NULL);attach_fill(s->calendar_book,NULL,bar);
    XtAddCallback(s->calendar_book,XmNpageChangedCallback,calendar_changed,s);
    static const char *captions[]={"Day","Week","Month","Year"};
    for(unsigned i=0;i<4;i++){char name[32];snprintf(name,sizeof(name),"calendarTab%u",i+1);Widget tab=XtVaCreateManagedWidget(name,xmPushButtonWidgetClass,s->calendar_book,XmNnotebookChildType,XmMAJOR_TAB,XmNpageNumber,i+1,NULL);label(tab,captions[i]);}
    Widget day=calendar_page(s->calendar_book,"dayView",1);s->day_label=XtVaCreateManagedWidget("dayDate",xmLabelWidgetClass,day,XmNappearanceRole,XmAPPEARANCE_ROLE_PAGE_TITLE,NULL);
    Widget note=XtVaCreateManagedWidget("dayNote",xmLabelWidgetClass,day,NULL);label(note,"Today's civil date. Choose Week, Month or Year to change the calendar range.");
    Widget week=calendar_page(s->calendar_book,"weekView",2);
    for(unsigned i=0;i<7;i++){char name[32];snprintf(name,sizeof(name),"weekDay%u",i);s->week_labels[i]=XtVaCreateManagedWidget(name,xmLabelWidgetClass,week,XmNalignment,XmALIGNMENT_BEGINNING,NULL);}
    Widget month=calendar_page(s->calendar_book,"monthView",3);s->calendar_month=XtVaCreateManagedWidget("monthCalendar",xmCalendarWidgetClass,month,NULL);
    Widget year=XtVaCreateManagedWidget("yearView",xmScrolledWindowWidgetClass,s->calendar_book,XmNnotebookChildType,XmPAGE,XmNpageNumber,4,XmNscrollingPolicy,XmAUTOMATIC,NULL);
    Widget grid=XtVaCreateManagedWidget("yearMonths",xmRowColumnWidgetClass,year,XmNorientation,XmVERTICAL,XmNpacking,XmPACK_COLUMN,XmNnumColumns,3,NULL);
    double scale=1.0;XmGetWidgetScale(s->root,&scale);
    for(unsigned i=0;i<12;i++){char name[32];snprintf(name,sizeof(name),"yearMonth%u",i+1);s->calendar_year[i]=XtVaCreateManagedWidget(name,xmCalendarWidgetClass,grid,XmNwidth,(Dimension)(260*scale),XmNheight,(Dimension)(220*scale),NULL);}
    fit_dialog(s,s->calendar,900,650);
}
static void create_confirmations(PeriodicActions *s)
{
    s->unsaved=XmCreateQuestionDialog(s->root,"periodicUnsavedDialog",NULL,0);
    XtVaSetValues(s->unsaved,XmNautoUnmanage,False,XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL,NULL);
    XmString message=XmStringCreateLocalized("Save your changes before continuing?");XtVaSetValues(s->unsaved,XmNmessageString,message,NULL);XmStringFree(message);
    label(XmMessageBoxGetChild(s->unsaved,XmDIALOG_OK_BUTTON),"Save");label(XmMessageBoxGetChild(s->unsaved,XmDIALOG_CANCEL_BUTTON),"Cancel");label(XmMessageBoxGetChild(s->unsaved,XmDIALOG_HELP_BUTTON),"Discard");
    XtAddCallback(s->unsaved,XmNokCallback,unsaved_save,s);XtAddCallback(s->unsaved,XmNcancelCallback,unsaved_cancel,s);/* Discard is a button activation, never the F1 help semantic. */
    XtAddCallback(XmMessageBoxGetChild(s->unsaved,XmDIALOG_HELP_BUTTON),XmNactivateCallback,unsaved_discard,s);
    s->overwrite=XmCreateQuestionDialog(s->root,"periodicOverwriteDialog",NULL,0);title(s->overwrite,"Replace existing file?");
    XtVaSetValues(s->overwrite,XmNautoUnmanage,False,XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL,NULL);
    message=XmStringCreateLocalized("The file already exists. Replace its contents?");XtVaSetValues(s->overwrite,XmNmessageString,message,NULL);XmStringFree(message);
    label(XmMessageBoxGetChild(s->overwrite,XmDIALOG_OK_BUTTON),"Replace");label(XmMessageBoxGetChild(s->overwrite,XmDIALOG_CANCEL_BUTTON),"Cancel");XtUnmanageChild(XmMessageBoxGetChild(s->overwrite,XmDIALOG_HELP_BUTTON));
    XtAddCallback(s->overwrite,XmNokCallback,overwrite_yes,s);XtAddCallback(s->overwrite,XmNcancelCallback,overwrite_no,s);
}
static void template_preview(Widget w,XtPointer client,XtPointer call)
{ dispatch(client,w,PA_PREVIEW,call); }
static void sample_help(Widget w,XtPointer client,XtPointer call)
{PeriodicActions *s=client;(void)call;help_message(s,"Sample dialog help",context_description(w));}
PeriodicActions *PeriodicInstallActions(Widget root)
{
    PeriodicActions *s=(PeriodicActions *)XtCalloc(1,sizeof(*s));s->root=root;s->print_fd=-1;s->word_wrap=True;
    s->main=XtNameToWidget(root,"*modernAppMain");if(!s->main)s->main=XtNameToWidget(root,"*appMain");
    Widget sample=XtNameToWidget(root,"*sampleText"),template_text=XtNameToWidget(root,"*templateText");
    s->file=XtNameToWidget(root,"*fileDialog");s->arrow=XtNameToWidget(root,"*arrowB1");
    if(!s->main || !sample || !template_text || !s->file){XtFree((char *)s);return NULL;}
    s->status=XtVaCreateManagedWidget("actionStatus",xmLabelWidgetClass,s->main,XmNalignment,XmALIGNMENT_BEGINNING,XmNappearanceRole,XmAPPEARANCE_ROLE_SECONDARY,NULL);
    XtVaSetValues(s->main,XmNmessageWindow,s->status,NULL);
    create_editor(s);create_help(s);create_preview(s);create_calendar(s);create_confirmations(s);
    init_document(s,&s->document,sample,s->editor_text);init_document(s,&s->template_document,template_text,NULL);s->editing=&s->document;
    replace_document(&s->template_document,"A Motif message template.\n\nEdit this text, include a file, preview the message, or save it as a draft.\n",NULL);
    XtAddCallback(XtNameToWidget(root,"*templateDialog"),XmNokCallback,template_preview,s);
    replace_document(&s->document,"Motif widgets make a text document.\nmotif can be searched without matching case.\nMOTIF examples support word wrap and clipboard editing.\n",NULL);update_find(s,False);
    XtVaSetValues(s->file,XmNautoUnmanage,False,NULL);XtAddCallback(s->file,XmNokCallback,file_accept,s);XtAddCallback(s->file,XmNcancelCallback,file_cancel,s);
    static const char *arrows[]={"*arrowB1","*arrowB2","*arrowB3","*arrowB4"};
    for(unsigned i=0;i<XtNumber(arrows);i++){Widget arrow=XtNameToWidget(root,(char *)arrows[i]);if(arrow)XtAddCallback(arrow,XmNactivateCallback,arrow_used,s);}
    Widget toggle=XtNameToWidget(root,"*wrapButton");if(toggle)XmToggleButtonSetState(toggle,True,False);
    toggle=XtNameToWidget(root,"*caseButton");if(toggle)XmToggleButtonSetState(toggle,False,False);set_wrap(s);
    static const char *layouts[]={"*stackButton","*tileButton"};static const char *views[]={"*dayButton","*weekButton","*monthButton","*yearButton"};select_radio(s,layouts,2,0);select_radio(s,views,4,0);
    context_watch(s,root);
    for(unsigned i=0;i<XtNumber(sample_dialogs);i++){Widget item=XtNameToWidget(root,(char *)sample_dialogs[i]);if(item){context_watch(s,item);if(XmIsMessageBox(item) || XmIsSelectionBox(item))XtAddCallback(item,XmNhelpCallback,sample_help,s);}}
    context_watch(s,s->editor);context_watch(s,s->help);context_watch(s,s->preview);context_watch(s,s->calendar);
    Atom wm_delete=XmInternAtom(XtDisplay(root),"WM_DELETE_WINDOW",False);XmAddWMProtocolCallback(root,wm_delete,main_close,s);XtVaSetValues(root,XmNdeleteResponse,XmDO_NOTHING,NULL);
    XtAddCallback(root,XmNdestroyCallback,destroy_actions,s);status(s,"Ready. File/Edit use the Text document; Help > Tutorial explains the examples.");return s;
}
