#include <stdio.h>
#include <string.h>
#include "wsm_proto.h"
static WSMAttribute attributes[2];
static WSMConfigFormatData formats[3];
WSMConfigFormatData *_WSMGetConfigFormat(Display *display, int screen, WSMConfigFormatType type)
{ (void)display; (void)screen; return &formats[type]; }
WSMConfigFormatType _WSMGetConfigFormatType(Window window)
{ return !window ? WSM_GLOBAL_FMT : window & ~WIN_MASK ? WSM_ICON_FMT : WSM_WINDOW_FMT; }
#define CHECK(test) do { if (!(test)) { fprintf(stderr,"FAIL wire validation line %d\n",__LINE__); return 1; } } while(0)
static int valid(unsigned char *data, size_t size, WSMRequestType type, Boolean reply)
{ return _WSMValidateMessage(NULL, 0, data, size, type, reply); }
int main(void)
{
    unsigned char connect[] = {0,3,3,2,1};
    CHECK(valid(connect,sizeof(connect),WSM_CONNECT,False));
    for (size_t i = 0; i < sizeof(connect); ++i) CHECK(!valid(connect,i,WSM_CONNECT,False));
    CHECK(!valid(NULL,1,WSM_CONNECT,False));
    CHECK(valid(NULL,0,WSM_CONFIG_FMT,False));
    CHECK(valid(NULL,0,WSM_SET_STATE,True));
    unsigned char extension[] = {0,1,0,3,'a','b','c'};
    CHECK(valid(extension,sizeof(extension),WSM_EXTENSIONS,False));
    for (size_t i = 0; i < sizeof(extension); ++i) CHECK(!valid(extension,i,WSM_EXTENSIONS,False));
    unsigned char config[] = {1,0,0,0,1,0,1,'x',32,0,0,0};
    CHECK(valid(config,sizeof(config),WSM_CONFIG_FMT,True));
    for (size_t i=0;i<sizeof(config);++i) CHECK(!valid(config,i,WSM_CONFIG_FMT,True));
    config[8]=7; CHECK(!valid(config,sizeof(config),WSM_CONFIG_FMT,True));
    attributes[0].size=32; attributes[1].size=16; attributes[1].is_list=True;
    formats[WSM_WINDOW_FMT].num_attrs=2; formats[WSM_WINDOW_FMT].attr_list=attributes;
    unsigned char state[] = {0,1,0,0,0,1,0xc0,0,0,0,5,0,2,0,8,0,9};
    CHECK(valid(state,sizeof(state),WSM_SET_STATE,False));
    CHECK(valid(state,sizeof(state),WSM_GET_STATE,True));
    for (size_t i=0;i<sizeof(state);++i) CHECK(!valid(state,i,WSM_SET_STATE,False));
    state[6]=0xc1; CHECK(!valid(state,sizeof(state),WSM_SET_STATE,False));
    state[6]=0xc0; state[12]=255; CHECK(!valid(state,sizeof(state),WSM_SET_STATE,False));
    unsigned char pointer_reply[8]={0};
    CHECK(valid(pointer_reply,8,WSM_WM_POINTER,True));
    CHECK(!valid(pointer_reply,4,WSM_WM_POINTER,True));
    unsigned char empty_values[]={0};
    CHECK(valid(empty_values,1,WSM_REG_WINDOW,True));
    empty_values[0]=1; CHECK(!valid(empty_values,1,WSM_REG_WINDOW,True));
    puts("PASS bounded workspace wire messages: truncation, counts, masks, formats and lists");
    return 0;
}
