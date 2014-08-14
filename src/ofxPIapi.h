/*
 *  ofxPIapi.h
 *
 *  Created by Leslie Yang on 2014-08-14
 *
 *
 *  Copyright 2014 https://github.com/LeslieFire All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the author nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 *  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 *  OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *  OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  ************************************************************************************ */ 
#ifndef OFXPIAPI_H
#define OFXPIAPI_H

#include "ofMain.h"
#include "ofxXmlSettings.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#include <string.h>

#include "piapix.h"
#include "pilogex.h"

#define TAG_LEN 80


#pragma comment(lib, "piapi32.lib");
#pragma comment(lib, "pilog32.lib");

typedef struct
{
   char tagname[TAG_LEN+1];
   int32 point;
   char descriptor[27];
   char engunit[32];
   int32 display_prec;
   PIvaluetype pt_typex;
   float64 rval, arcrval;
   int32  ival, arcival;
   void  *bval, *arcbval;
   uint32 bsize, arcbsize;
   int32  istat, arcistat;
   int16  flags, arcflags;
   PITIMESTAMP ts, arcts;
} TAG;

#define MAXTRYS 5
#ifndef TRUE
#define TRUE -1
#endif
#ifndef FALSE
#define FALSE 0
#endif


#define BUFSIZE  1024

class ofxPIapi
{
    public:
        ofxPIapi();
        virtual ~ofxPIapi();

       int32    connection;
       char name[32], address[32];
       char     version[32];

        bool getAPIVersion();
        /** \brief
         *
         * \connectionMode =
         *  0: use default server node
         *  1: use piut_setservernode()
         *
         * \param
         * \return
         *
         */

        bool connect(bool connectionMode = 0 , string serverNode=" ");
        void disconnect();


        // dosnap函数不调用 print_taginfo, tag作为引用参数传进dosnap
        // 当dosnap执行完毕 tag被填充tname的所有相关数据

        int dosnap(const char *tname, TAG &tag);

        char *gettypestr(PIvaluetype pttype);
        char *getvaluestr(char *stat, int32 statlen, int32 *vallen,
                int32 point, PIvaluetype pt_typex, char *engunit, int32 dis_dig,
                double drval, int32 ival, void * bval, uint32 bsize, int32 istat);
        char * gettimestr( int32 istat, PITIMESTAMP pits);

        // 当执行完 dosnap后，执行print_taginfo函数，返回tag数据值
        // 当前仅没有返回 arc类的数据，仅作为测试使用
        string print_taginfo ( TAG *tag );

        int32 lastdayofmonth ( int32 month );
};

#endif // OFXPIAPI_H
