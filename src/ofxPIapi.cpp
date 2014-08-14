/*
 *  ofxPIapi.cpp
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
#include "ofxPIapi.h"

ofxPIapi::ofxPIapi()
{
    //ctor
    connection = FALSE;
}

ofxPIapi::~ofxPIapi()
{
    //dtor
    piut_disconnect();
}

void ofxPIapi::disconnect(){
    piut_disconnect();
}


bool ofxPIapi::connect(bool connectionMode, string serverNode){
    int32    result = 1;
    int      trys = 0;

    if (connectionMode == 1){
        /*  attempt connection to passed node name */

        piut_setprocname("snap");
        ofLogNotice() <<"Attempting connection to " + serverNode;

        while(result && trys < MAXTRYS){
            trys ++;
            result = piut_setservernode(serverNode.c_str());
            if (result){
                ofLogError() <<"Error : " + ofToString((long)result) + ", MAXTRYS ,Connecting to " + serverNode;
                if ( trys == MAXTRYS){
                    return false;
                }
            }
            else{
                connection = TRUE;
                return true;
            }
        }
    } else if (connectionMode == 0){
        /*  attempt connection to default homenode */
         ofLogNotice() <<"Attempting connection to Default homenode";
         //printf ( "\n Attempting connection to Default homenode" );
         while ( result && trys < MAXTRYS )
         {
            trys++;
            result = piut_connect ( "snap" );
            if ( result )
            {
                ofLogNotice() <<"Error " + ofToString(result)  + " connecting to Default homenode";
               //printf ( "\n Error %ld, connecting to Default homenode", (long)result );
               return result;
            }
            else
            {
               if ( piut_netserverinfo ( name, 32, address,
                  32, &connection ) )
               {
                   ofLogNotice() <<"Connected to DEFAULT pihome node";
                  //printf ( "\nConnected to DEFAULT pihome node" );
                  connection = TRUE;
               }
               else
               {
                   ofLogNotice() <<"Connected to " + ofToString(name) + " : " + ofToString(address);
                  //printf ( "\nConnected to %s (%s)", name, address );
               }
            }
         }

    }
    if ( !connection){
        ofLogError() <<"Error " + ofToString((long)result) + "connecting to PI Server node";
        return false;
    }
    return true;
}

int ofxPIapi::dosnap(const char *tname, TAG &tag){
    int32 result, reterr, pt_count;
   int len, once_len;
   float64 *pSnapDval, *pArcDval;
   int32   *pSnapIval, *pArcIval;
   void    *pSnapBval = NULL;
   void    *pArcBval = NULL;
   uint32  tmp_bsize;
//   TAG tag;
   tag.bsize = 0;
   tag.arcbsize = 0;
   tag.bval = NULL;
   tag.arcbval = NULL;

   strcpy(tag.tagname, tname);
   once_len = strlen( tag.tagname );
   len = once_len;
 //  while ( TRUE )
 //  {
      if ( len > 0 )
      {
         if (tag.tagname[0] == '\\')
         {
            tag.point = atoi( &tag.tagname[1] );
            result = 0;
         }
         else
            result = pipt_findpoint (tag.tagname, &tag.point );
         switch ( result )
         {
         default:
             // add by leslie @ 2014/7/24  -------------@begin
             ofLogError()<<"pipt_findpoint error " + ofToString(result) + ", tag " + tag.tagname;
             // add by leslie @ 2014/7/24  -------------@end
            printf ( "\npipt_findpoint error %ld, tag %s",
               result, tag.tagname );
            piut_disconnect();
            return -1;
         case -5:
         case -2:
             // add by leslie @ 2014/7/24  -------------@begin
             ofLogError()<<"Tag " + ofToString(tag.tagname) + " does not exist";
             // add by leslie @ 2014/7/24  -------------@end

            printf ( "\nTag %s does not exist", tag.tagname );
            break;
         case 0:
            result = pipt_tagpreferred ( tag.point, tag.tagname, TAG_LEN+1 );
            if ( result )
            {
                // add by leslie @ 2014/7/24  -------------@begin
                ofLogError()<<"pipt_tag " + ofToString(result);
                // add by leslie @ 2014/7/24  -------------@end
               printf ( "\npipt_tag %ld", result );
            }
            result = pipt_descriptor ( tag.point, tag.descriptor, 27 );
            if ( result )
            {
                // add by leslie @ 2014/7/24  -------------@begin
                ofLogError()<<"pipt_descriptor " + ofToString(result);
                // add by leslie @ 2014/7/24  -------------@end
               printf ( "\npipt_descriptor %ld", result );
               tag.descriptor[0] = '\0';
            }
            result = pipt_engunitstring ( tag.point, tag.engunit, 32 );
            if ( result )
            {
                // add by leslie @ 2014/7/24  -------------@begin
                ofLogError()<<"pipt_engunitstring " + ofToString(result);
                // add by leslie @ 2014/7/24  -------------@end
               printf ( "\npipt_engunitstring %ld", result );
               tag.engunit[0] = '\0';
            }
            result = pipt_pointtypex ( tag.point, &tag.pt_typex );
            if ( result )
            {
                // add by leslie @ 2014/7/24  -------------@begin
                ofLogError()<<"pipt_pointtypex " + ofToString(result);
                // add by leslie @ 2014/7/24  -------------@end
               printf ( "\npipt_pointtypex %ld", result );
               tag.pt_typex = PI_Type_bad;
            }
            result = pipt_displaydigits(tag.point, &tag.display_prec);
            if ( result )
            {
                // add by leslie @ 2014/7/24  -------------@begin
                ofLogError()<<"pipt_displaydigits " + ofToString(result);
                // add by leslie @ 2014/7/24  -------------@end
               printf ( "\npipt_displaydigits %ld", result );
               tag.display_prec = -5;
            }
            tag.rval = (float) 0.0;
            switch (tag.pt_typex ) {
            case PI_Type_PIstring:
            case PI_Type_blob:
            case PI_Type_PItimestamp:
               pSnapDval = pArcDval = NULL;
               pSnapIval = pArcIval = NULL;
               /*--- Snapshot ---*/
               /* Start with BUFSIZE and reallocate if needed. */
               if (tag.bsize == 0) {/* Skip allocation if a subsequent run. */
                  if ( (tag.bval = (void *)malloc(BUFSIZE))==NULL ) {
                     // add by leslie @ 2014/7/24  -------------@begin
                    ofLogError()<<"Error allocating bval storage ";
                    // add by leslie @ 2014/7/24  -------------@end
                     printf("\nError allocating bval storage");
                     piut_disconnect();
                     exit(1);
                  }
               }
               tag.bsize = (tag.bsize > BUFSIZE - 1) ? tag.bsize : BUFSIZE - 1;
               memset(tag.bval, 0, (size_t)(tag.bsize+1));
               pSnapBval = tag.bval;
               /*--- Archive ---*/
               if (tag.arcbsize == 0) {
                  if ( (tag.arcbval = (void *)malloc(BUFSIZE))==NULL ) {
                        // add by leslie @ 2014/7/24  -------------@begin
                    ofLogError()<<"Error allocating bval storage ";
                    // add by leslie @ 2014/7/24  -------------@end
                     printf("\nError allocating bval storage");
                     exit(1);
                  }
               }
               tag.arcbsize = (tag.arcbsize > BUFSIZE - 1) ? tag.arcbsize : BUFSIZE - 1;
               memset( tag.arcbval, 0, (size_t)(tag.arcbsize+1));
               pArcBval = tag.arcbval;
               break;
            case PI_Type_int16:
            case PI_Type_int32:
               pSnapDval = pArcDval = NULL;
               pSnapBval = pArcBval = NULL;
               tag.bsize = tag.arcbsize = 0;
               pSnapIval = &tag.ival;
               pArcIval  = &tag.arcival;
               break;
            case PI_Type_digital:
               pSnapDval = pArcDval = NULL;
               pSnapIval = pArcIval = NULL;
               pSnapBval = pArcBval = NULL;
               tag.bsize = tag.arcbsize = 0;
               break;
            default:/* floats, PI2 */
               pSnapIval = pArcIval = NULL;
               pSnapBval = pArcBval = NULL;
               tag.bsize = tag.arcbsize = 0;
               pSnapDval = &tag.rval;
               pArcDval  = &tag.arcrval;
               break;
            } /* End switch */

            /*--- Snapshot ---*/
            pt_count = 1;
            result = pisn_getsnapshotsx ( &tag.point, &pt_count, pSnapDval,
               pSnapIval, pSnapBval, &tag.bsize, &tag.istat, &tag.flags,
               &tag.ts, &reterr, GETFIRST);
            if (result == -15010/*PI_OVERFLOW*/) {
               if ( tag.bsize > 4096)
               {
                  printf("\nError %ld; returned bsize= %ld",
                     (long)result, (long)tag.bsize);
                  free(tag.bval);
                  free(tag.arcbval);
                  exit(1);
               }
               if ((tag.bval= (void *)realloc(tag.bval,(size_t)(tag.bsize+1)))==
                  NULL ) {
                  printf("\nError allocating bval storage");
                  free(tag.bval);
                  free(tag.arcbval);
                  exit(1);
               }
               memset( tag.bval, 0,(size_t)(tag.bsize+1));
               pSnapBval = tag.bval;
               pt_count = 1;
               result = pisn_getsnapshotsx ( &tag.point, &pt_count, pSnapDval,
                  pSnapIval, pSnapBval, &tag.bsize, &tag.istat, &tag.flags,
                  &tag.ts, &reterr, GETSAME);
            }
            if (result) {
               if (/*PI_WARNING*/-30000==result) result = reterr;
               tag.istat = 0xffffffff;  /* mark as unsuccessful call 5feb97 */
               printf ( "\nError:  pisn_getsnapshotsx %ld", result );
               pitm_setcurtime(&tag.ts, FALSE);
            }

            /*--- Archive ---*/
            memcpy((void *)&tag.arcts,(void *)&tag.ts, sizeof(tag.ts));
            /* Retrieve timestamp of a recent archive event using piar_getarcvaluex */
            /* Subtract approximately one minute from the snapshot time. The timestamp */
			/* retrieved need not be the previous since the next archive call will allow */
			/* for this. */
            tag.arcts.second = 0.0;
			if (tag.arcts.minute > 0) {
               tag.arcts.minute -= 1;
            } else if (tag.arcts.hour > 0) {
				tag.arcts.hour -= 1;
				tag.arcts.minute = 59;
			} else if (tag.arcts.day > 1) {
				tag.arcts.day -= 1;
				tag.arcts.hour = 23;
				tag.arcts.minute = 59;
			} else if (tag.arcts.month > 1) {
				tag.arcts.month -= 1;
				tag.arcts.day = lastdayofmonth(tag.arcts.month);
				tag.arcts.hour = 23;
				tag.arcts.minute = 59;
			} else {
				tag.arcts.year -= 1;
				tag.arcts.month = 12;
				tag.arcts.day = lastdayofmonth(tag.arcts.month);
				tag.arcts.hour = 23;
				tag.arcts.minute = 59;
			}

            /* Obtain the timestamp alone by passing all other data arguments as null */
			result = piar_getarcvaluex(tag.point, ARCVALUEBEFORE, 0,
               0, 0, 0, 0, 0, &tag.arcts);

            if (result) {
               tag.istat = 0xffffffff;  /* mark as unsuccessful call */
               printf ( "\nError:  piar_getarcvaluex %ld", result );
               pitm_setcurtime(&tag.arcts, FALSE);
            }

            /* Retrieve last archive value using piar_getarcvaluesx */
			/* This will allow retrieval of the previous archive event even if */
			/* it has the same timestamp and possibly the same value as the snapshot. */
            /* Use the timestamp retrieved above. It will be sufficient to limit */
			/* the range of time queried. Limiting the range is not an issue for PI, */
            /* but can be damaging for some foreign systems mapped to PI using UDA that */
			/* honor the time range before the requested value count. */
            pt_count = 2;
            tmp_bsize = tag.arcbsize;
            result = piar_getarcvaluesx(tag.point, ARCflag_comp, &pt_count,
               pArcDval, pArcIval, pArcBval, &tag.arcbsize, &tag.arcistat,
               &tag.arcflags, &tag.ts, &tag.arcts, GETFIRST);
            /* Ignore snapshot and go to next value : clear buffer. */
            if (tag.arcbsize > 0 && (result == 0 || result == -15010)) {
               tag.arcbsize = tmp_bsize;
               memset(tag.arcbval, 0, (size_t)tag.arcbsize);
            }
            result = piar_getarcvaluesx(tag.point, ARCflag_comp, &pt_count,
               pArcDval, pArcIval, pArcBval, &tag.arcbsize, &tag.arcistat,
               &tag.arcflags, &tag.ts, &tag.arcts, GETNEXT);
            if (result == -15010 /*PI_OVERFLOW*/) {
               if ( (tag.arcbval = (void *)realloc(tag.arcbval,
                  (size_t)(tag.arcbsize+1)))==NULL ) {
                  printf("\nError allocating bval storage");
                  exit(1);
               }
               memset(tag.arcbval, 0, (size_t)(tag.arcbsize+1));
               pArcBval = tag.arcbval;
               result = piar_getarcvaluesx(tag.point, ARCflag_comp, &pt_count,
                  pArcDval, pArcIval, pArcBval, &tag.arcbsize, &tag.arcistat,
                  &tag.arcflags, &tag.ts, &tag.arcts, GETSAME);
            }

            if (result) {
               tag.arcistat = 0xffffffff;  /* mark as unsuccessful call 5feb97 */
               printf ( "\nError:  piar_getarcvaluesx %ld", result );
            }

            //print_taginfo ( &tag );
            break;
         }
 //        if (once_len > 0) break;
      }
      /*
      printf ( "\n\nEnter tagname:  " );
      gets ( tag.tagname );
      len = strlen ( tag.tagname );
      if ( len < 1 )
         break;
         */
 //  }
    /*
    if (tag.bval) free(tag.bval);
   if (tag.arcbval) free(tag.arcbval);*/
   return ( 0 );
}

char *ofxPIapi::gettypestr(PIvaluetype pttype){
   static char  tmpstr[8];
   switch (pttype)
   {
   case PI_Type_int16:
      return "Integer-16";
   case PI_Type_int32:
      return "Integer-32";
   case PI_Type_digital:
      return "Digital";
   case PI_Type_PIstring:
      return "String";
   case PI_Type_PItimestamp:
      return "Timestamp";
   case PI_Type_blob:
      return "Blob";
   case PI_Type_float32:
      return "Real-32";
   case PI_Type_float16:
      return "Real-16";
   case PI_Type_float64:
      return "Real-64";
   case PI_Type_PI2:
      return "PI2";
   default:
      sprintf(tmpstr, "%d", (int)pttype);
      break;
   }
   return tmpstr;
}
char *ofxPIapi::getvaluestr(char *stat, int32 statlen, int32 *vallen,
                int32 point, PIvaluetype pt_typex, char *engunit, int32 dis_dig,
                double drval, int32 ival, void * bval, uint32 bsize, int32 istat)
{
   static char  tmpstr[256];
   int32 digcode, dignumb, result, localistat;
   if ( istat == 0xffffffff )
   {
      strncpy ( stat, "ERROR", statlen );

      stat[statlen-1] = '\0';
      *vallen = 5;
      return "ERROR";
   }
   else if ( istat != 0 )
   {
      localistat = istat;
      if (pt_typex == PI_Type_digital)
      {
         result = pipt_digpointers ( point, &digcode, &dignumb );
         if (!result && istat >= 0 && istat <= dignumb )
            localistat += digcode;
      }
      result = pipt_digstate ( localistat, stat, statlen );
      if ( result )
         strncpy ( stat, "-----", statlen );

      stat[statlen-1] = '\0';
      *vallen = strlen(stat);
      return stat;
   }
   strncpy ( stat, "Good", statlen );
   stat[statlen-1] = '\0';
   switch ( pt_typex )
   {
   case PI_Type_int16:
   case PI_Type_int32:
      sprintf ( tmpstr, "%11ld %s", ival, engunit );
      *vallen = strlen(tmpstr);
      break;

   case PI_Type_blob:
      sprintf ( tmpstr, "N/A   Size = %lu", (unsigned long)bsize );
      *vallen = strlen(tmpstr);
      break;

   case PI_Type_PItimestamp:
   case PI_Type_PIstring:
      *vallen = bsize;
      return (char *)bval;

   default:
   case PI_Type_float32:
   case PI_Type_float16:
   case PI_Type_float64:
   case PI_Type_PI2:
      /* TODO: implement precision attribute */
      if (dis_dig >= 0) {
         sprintf ( tmpstr, "%.*f %s", (int)dis_dig, drval, engunit );
      } else {
         sprintf ( tmpstr, "%.*g %s", (int)(-dis_dig), drval, engunit );
      }
      *vallen = strlen(tmpstr);
      break;
   }/* End switch pt_typex */
   return tmpstr;
}
char * ofxPIapi::gettimestr( int32 istat, PITIMESTAMP pits)
{
   static char tstr[32];
   static char mstr[12][4] = {"Jan","Feb","Mar","Apr","May","Jun",
      "Jul","Aug","Sep","Oct","Nov","Dec"};
   double frac;
   if ( istat == 0xffffffff )
      return "ERROR";
   else {
      sprintf(tstr,"%02d-%s-%02d %02d:%02d:%02d",
         pits.day, mstr[pits.month-1],
         ( (pits.year>=2000)?pits.year-2000 : pits.year-1900 ),
         pits.hour, pits.minute, (int)pits.second);
      frac = (pits.second-(int32)pits.second);
      if ( frac > 0.00001)
         sprintf(tstr, "%s.%05d", tstr, (int)(frac*100000.0));
   }
   return tstr;
}

string ofxPIapi::print_taginfo ( TAG *tag )
{
    int32 valpos;
   char stat[80];
   char arcstat[80];
// add by leslie @ 2014/7/24 -------------@begin
    string ret = getvaluestr(stat, sizeof(stat), &valpos,
      tag->point, tag->pt_typex, tag->engunit, tag->display_prec,
                tag->rval, tag->ival, tag->bval, tag->bsize, tag->istat);

    if (tag->bval) free(tag->bval);
   if (tag->arcbval) free(tag->arcbval);
// add by leslie @ 2014/7/24 -------------@end


/*
   printf ( "\n\n  Tag = %s   Point Number = %ld   Type = %s",
      tag->tagname, tag->point, gettypestr(tag->pt_typex) );
   printf ( "\n  %s", tag->descriptor );
   printf ( "\n\n                Snapshot value" );
   printf ( "\n  Value = %s", getvaluestr(stat, sizeof(stat), &valpos,
      tag->point, tag->pt_typex, tag->engunit, tag->display_prec,
                tag->rval, tag->ival, tag->bval, tag->bsize, tag->istat));
   if (valpos < 50)
      printf("\t%s", gettimestr(tag->istat, tag->ts) );
   else
      printf("\n  Time = %s", gettimestr(tag->istat, tag->ts) );

   if ( tag->pt_typex != PI_Type_digital)
      printf ( "\n  Status = %s", stat );
   if (tag->flags > 0) {
      stat[0] = '\0';// Reuse stat.
      // Allow for combinations of flags by check each individually.
      if ( (tag->flags & PI_M_AFLAG) )
         strcat ( stat, " Annotated");
      if ( (tag->flags & PI_M_QFLAG) )
         strcat ( stat, " Questionable" );
      if ( (tag->flags & PI_M_SFLAG) )
         strcat ( stat, " Substituted" );
      printf ( "\n  Flags =%s", stat );
   }

   printf ( "\n\n              Latest archive value" );
   printf ( "\n  Value = %s", getvaluestr(arcstat, sizeof(arcstat), &valpos,
      tag->point, tag->pt_typex, tag->engunit, tag->display_prec,
                tag->arcrval, tag->arcival, tag->arcbval, tag->arcbsize, tag->arcistat));
   if (valpos < 50)
      printf("\t%s", gettimestr(tag->arcistat, tag->arcts) );
   else
      printf("\n  Time = %s", gettimestr(tag->arcistat, tag->arcts) );

   if ( tag->pt_typex != PI_Type_digital)
      printf ( "\n  Status = %s", arcstat );

   if (tag->arcflags > 0) {
      arcstat[0] = '\0';// Reuse stat.
      // Allow for combinations of flags by check each individually.
      if ( (tag->arcflags & PI_M_AFLAG) )
         strcat ( arcstat, " Annotated");
      if ( (tag->arcflags & PI_M_QFLAG) )
         strcat ( arcstat, " Questionable" );
      if ( (tag->arcflags & PI_M_SFLAG) )
         strcat ( arcstat, " Substituted" );
      printf ( "\n  Flags =%s\n", arcstat );
   } else printf("\n");

*/
   return ret;
}

int32 ofxPIapi::lastdayofmonth ( int32 month )
{
	switch (month) { /* month is 1-12 */
	case 1: /* jan */
	case 3: /* mar */
	case 5: /* may */
	case 7: /* jul */
	case 8: /* aug */
	case 10:  /* oct */
	case 12: /* dec */
		return 31;

	case 4: /* apr */
	case 6: /* jun */
	case 9: /* sep */
	case 11:  /* nov */
		return 30;

	case 2: /* feb */
	default: /* not worth worrying about leap years */
		return 28; /* return lowest legal day for all months */
	}
}

 bool ofxPIapi::getAPIVersion(){
    if ( !piut_getapiversion ( version, sizeof(version) ) ){
        ofLogNotice() <<"PI-API version " + ofToString(version) ;
        return true;
    }
    return false;
 }
