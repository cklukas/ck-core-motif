#!/usr/bin/env python3
"""Exercise production command-tree deletion under ASan/UBSan."""
import os
from pathlib import Path
import subprocess
import tempfile
root=Path(__file__).resolve().parents[2]
text=(root/'clients/mwm/WmCmd.c').read_text()
def extract(name):
    start=text.index('void\n'+name+'(')
    end=text.index('/*---',start)
    return text[start:end]
program='''#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef uint32_t CARD32;
typedef struct CmdTree { CARD32 commandID; char *name,*defaultName; struct CmdTree *next,*subTrees; } CmdTree;
static unsigned live;
static void *owned(size_t size){void *value=calloc(1,size);if(!value)abort();++live;return value;}
static void XtFree(char *value){if(value){--live;free(value);}}
static CmdTree *node(CARD32 id){CmdTree *value=owned(sizeof(*value));value->commandID=id;
 value->name=owned(12);strcpy(value->name,"command");value->defaultName=owned(12);strcpy(value->defaultName,"label");return value;}
'''+extract('RemoveCommandBranch')+extract('DeleteCommand')+'''
int main(void){
 CmdTree *root=node(1);root->next=node(2);root->next->next=node(3);
 root->subTrees=node(4);root->subTrees->next=node(5);root->subTrees->next->subTrees=node(6);
 DeleteCommand(1,&root);if(!root||root->commandID!=2)return 1;
 root->subTrees=node(7);root->subTrees->next=node(7);root->next->subTrees=node(7);
 DeleteCommand(7,&root);if(root->subTrees||root->next->subTrees)return 2;
 DeleteCommand(999,&root);DeleteCommand(0,NULL);
 RemoveCommandBranch(root);if(live){fprintf(stderr,"FAIL leaked %u allocations\\n",live);return 3;}
 puts("PASS command-tree head, sibling, nested and repeated-ID deletion; all names and labels released");return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='mwm-command-tree-') as temporary:
    source=Path(temporary)/'test.c';binary=Path(temporary)/'test';source.write_text(program)
    subprocess.run([os.environ.get('CC','cc'),'-std=c99','-Wall','-Wextra','-Werror',
                    '-fsanitize=address,undefined','-fno-omit-frame-pointer',str(source),'-o',str(binary)],check=True)
    subprocess.run([str(binary)],check=True)
