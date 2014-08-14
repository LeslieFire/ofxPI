/*
 *  ThreadedPIData.cpp
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
#include "ThreadedPIData.h"


ofEvent <ThreadedPIDataEvent> ThreadedPIDataEvent::events;


ThreadedPIData::~ThreadedPIData()
{
    //dtor
}

void ThreadedPIData::setup(string filename){

    serverNode1 = "10.136.32.6";
    serverNode2 = "10.150.156.22";

    tagNum.resize(TABLENUM);
    tagNum[0] = 50;
    tagNum[1] = 27;
    tagNum[2] = 41;

    if (xml.loadFile(filename)){

        serverNode1 = xml.getValue("ServerNode1", serverNode1);
        serverNode2 = xml.getValue("ServerNode2", serverNode2);

        tagNum[0] = xml.getValue("TableOneTagNumber", 0);
        tagNum[1] = xml.getValue("TableTwoTagNumber", 0);
        tagNum[2] = xml.getValue("TableThreeTagNumber", 0);

        int numTableTags = xml.getNumTags("TABLE:TAG");

 //       ofLogVerbose() << "TABLE tag number : " + ofToString(numTableTags);

        for (int i = 0 ; i < numTableTags; ++i){
            xml.pushTag("TABLE", i);

                int numTags = xml.getNumTags("TAG");
 //               ofLogVerbose() << "TAG tag number : " + ofToString(numTableTags);

                if (numTags > 0){
                    for (int j = 0; j < tagNum[i]; ++j){
                        serNum[i].push_back(xml.getValue("SERVERNUMBER", 1, j));
                        tagname[i].push_back(xml.getValue("TAG", " ", j));
                    }
                }
            xml.popTag();
        }

    }else{
        xml.setValue("ServerNode1", serverNode1);
        xml.setValue("ServerNode2", serverNode2);

        xml.setValue("TableOneTagNumber", tagNum[0]);
        xml.setValue("TableTwoTagNumber", tagNum[1]);
        xml.setValue("TableThreeTagNumber", tagNum[2]);

        int lastTagNumber = 0;
        for (int i = 0; i < TABLENUM; ++i){
            lastTagNumber = xml.addTag("TABLE");
            if (xml.pushTag("TABLE", lastTagNumber)){
                for (int j = 0; j < tagNum[i]; ++j){
                    int serNum = xml.addTag("SERVERNUMBER");
                    xml.setValue("SERVERNUMBER", 1, serNum);
                    int tagNum = xml.addTag("TAG");
                    xml.setValue("TAG", "tname"+ofToString(i)+ofToString(j), tagNum);
                }
                xml.popTag();
            }
        }

        xml.saveFile(filename);
    }

    if ( !PIPC.getAPIVersion() ){
        ofLogError() <<"PIPC1:Error to get API version!" ;
    }
}

bool ThreadedPIData::getData(vector<string> temp[]){
    if (!PIPC.connect(1, serverNode1)){
        ofLogError() <<"Failed to connect to server node 1.";
        static ThreadedPIDataEvent disEvent1;
        disEvent1.bConnected = false;
        disEvent1.message = "Failed to connect to server : " + serverNode1;

        ofNotifyEvent(ThreadedPIDataEvent::events, disEvent1);

        return false;
    }else{
        getServerData(1, LOAD, temp[LOAD]);
        getServerData(1, ENVIRONMENT, temp[ENVIRONMENT]);
        getServerData(1, OPERATION_STATUS, temp[OPERATION_STATUS]);

        PIPC.disconnect();
    }

    if (!PIPC.connect(1, serverNode2)){
        ofLogError() <<"Failed to connect to server node 2.";

        static ThreadedPIDataEvent disEvent2;
        disEvent2.bConnected = false;
        disEvent2.message = "Failed to connect to server : " + serverNode2;

        ofNotifyEvent(ThreadedPIDataEvent::events, disEvent2);
        return false;
    }else{
        getServerData(2, LOAD, temp[LOAD]);
        getServerData(2, ENVIRONMENT, temp[ENVIRONMENT]);
        getServerData(2, OPERATION_STATUS, temp[OPERATION_STATUS]);

        PIPC.disconnect();
    }

    return true;
}

void ThreadedPIData::getServerData(int serverNum, dataType type, vector<string> &temp){
    for (int i = 0 ; i < tagname[type].size(); ++i){
        if (serNum[type].at(i) == serverNum){
            TAG tag;
            string tname = tagname[type].at(i);
            if (PIPC.dosnap(tname.c_str(), tag) == -1){
                ofLogError() <<"do snap error with tag name : " + tname;
            }
            temp[i] = PIPC.print_taginfo(&tag);
        }
    }
}
