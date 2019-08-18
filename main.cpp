/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "EvHttpServ.h"
#include "Utilis/Defer.h"
#include "Utilis/Logger.h"
#include "Utilis/Initialize.h"

#ifdef WIN32
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib") 
#pragma comment(lib,"wsock32.lib")
#pragma   warning(disable : 4290) 
#endif

using namespace Network;

void testGetHandler(EvHttpResp *resp)
{
	Utilis::LogInfo("Get respuest, start handlering...\n");
    std::string strHost = resp->GetUriHost();  
    int nPort =  resp->GetUriPort();
    std::string strPath = resp->GetUriPath();
    std::string strQuery = resp->GetUriQuery();
    std::cout<<"Get port:"<<nPort<<std::endl;
    std::cout<<"Get uri:"<<strPath<<std::endl;
    std::cout<<"Get uri params:"<<strQuery<<std::endl;

    std::string gKey("getparam");
    std::string hKey("pathparam");
    std::string pKey("postparam");
    std::string strGVal = resp->GetPathParam(gKey);
    std::string strHVal = resp->GetHeadParam(hKey);
    std::string strPVal = resp->GetPostParam(pKey);
    std::cout<<"Get path param:"<<strGVal<<std::endl;
    std::cout<<"Get head param:"<<strHVal<<std::endl;
    std::cout<<"Get post param:"<<strPVal<<std::endl;

    std::string strBody = resp->GetPostMsg();

    std::string rKey("retHead");
    std::string rVal("retValue");
    std::string rBody("Hello World!\n");
    resp->AddRespHeadParam(rKey,rVal);
    resp->AddRespString(rBody);
    std::string rBuf("Winter is over!\n");
    resp->AddRespBuf(rBuf.c_str(),rBuf.length());

    resp->SetRespCode(200);
    resp->SendResponse();
}

int main()
{
#ifdef WIN32
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0){
		return 0;
	}

	Utilis::DEFER([]{ WSACleanup(); });
#endif
	Utilis::Logger::GetInstance()->StartLogging();
	Utilis::DEFER([] { Utilis::Logger::GetInstance()->StopLogging(); });

    short http_port = 8077;
    std::string http_addr("0.0.0.0\0");
    EvHttpServ Serv(http_addr,http_port);

    Serv.RegistHandler("/hi/testget", testGetHandler);

	Utilis::LogInfo("Starting server...\n");
    Serv.Start();
	
    return 0;
} 
