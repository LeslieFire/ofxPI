/*
 *  ThreadedPIData.h
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
#ifndef THREADEDPIDATA_H
#define THREADEDPIDATA_H

#include <ofMain.h>
#include <ofxXmlSettings.h>
#include "ofxPIapi.h"

enum dataType{
    LOAD,
    ENVIRONMENT,
    OPERATION_STATUS
};

const int TABLENUM = 3;
const int TAGNUM = 100;


class ThreadedPIDataEvent : public ofEventArgs {
    public:

    vector<string> * newData;
    bool            bConnected;
    string   message;

    ThreadedPIDataEvent() {
        newData = NULL;
        bConnected = true;
    }

    static ofEvent <ThreadedPIDataEvent> events;
};

class ThreadedPIData : public ofThread
{
    public:
        ThreadedPIData(): bNewData(false), bConnected(false){

        }
        virtual ~ThreadedPIData();

        void setup(string filename);

        bool isNewData() const{
            return bNewData;
        }

        void setNewData(bool b){
            if(lock()){
                ofLogNotice() <<"Set false to bNewData";
                bNewData = b;
                unlock();
            }
        }

        vector<string> getPIData(dataType type) const{
            // is there need a lock()?
            return piData[type];
        }

        void start(){
            startThread(true, false);   // blocking, verbose
        }

        void stop(){
            stopThread();
        }

		//--------------------------
		void threadedFunction(){

			while( isThreadRunning() != 0 ){
				if( lock() ){
                    vector<string>  temp[TABLENUM];

                    temp[0].resize(tagNum[LOAD]);
                    temp[1].resize(tagNum[ENVIRONMENT]);
                    temp[2].resize(tagNum[OPERATION_STATUS]);
                    if (piData[0].size() > 0){
                        // there is old data
                        temp[0].assign(piData[0].begin(), piData[0].end());
                        temp[1].assign(piData[1].begin(), piData[1].end());
                        temp[2].assign(piData[2].begin(), piData[2].end());
                    }

                    if (getData(temp)){
                        piData[0].assign(temp[0].begin(), temp[0].end());
                        piData[1].assign(temp[1].begin(), temp[1].end());
                        piData[2].assign(temp[2].begin(), temp[2].end());

                        bNewData = true;

                        static ThreadedPIDataEvent newEvent;
                        newEvent.newData = piData;
                        newEvent.message = "";

                        ofNotifyEvent(ThreadedPIDataEvent::events, newEvent);
                    }


					unlock();
					ofSleepMillis(1 * 200);
				}
			}
		}

//    protected:


 //   private:
        bool getData(vector<string> temp[]);
        void getServerData(int serverNum, dataType type, vector<string> &temp);

        bool            bNewData;       // Is there new data or not
        bool            bConnected;
        dataType        tableType;      // which table is showing now
        ofxPIapi        PIPC;           // PI - api
        vector<string>  piData[TABLENUM];

        string          serverNode1; // server node 1: 10.136.32.6
        string          serverNode2; // server node 2: 10.150.156.22

		ofTrueTypeFont  data;

		ofxXmlSettings  xml;

        vector<int> tagNum;
        vector<string> tagname[TABLENUM];
        vector<int>    serNum[TABLENUM];

        int   connectionMode;   // 1 : server node  ; 0: default home nodeint   connectionMode;   // 1 : server node  ; 0: default home node
};

#endif // THREADEDPIDATA_H
