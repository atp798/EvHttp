/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>

#include "EvHttpServ.h"
#include "EvHttpResp.h"

#include "Utilis/Logger.h"

#include "evhttp.h"
#include "event.h"
#include "event2/http.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http_struct.h"
#include "event2/http_compat.h"
#include "event2/util.h"
#include "event2/listener.h"

#include <arpa/inet.h>

#ifdef WIN32
#include <WinSock2.h>
#endif

namespace Network
{
EvHttpServ::EvHttpServ():EvHttpServ("",0){
}

EvHttpServ::~EvHttpServ(){
    //#TODO Works only if no requests are currently being served.
    if(evHttp_){ evhttp_free(evHttp_); }
    if(evBase_ ){ event_base_free(evBase_);}
}

void evLogCB(int severity, const char* msg) throw(std::runtime_error){
	switch (severity) {
	case EVENT_LOG_DEBUG:
		Utilis::Logger::GetInstance()->SimpleAppendLog(Utilis::LL_DEBUG, msg);
		break;
	case EVENT_LOG_MSG:
		Utilis::Logger::GetInstance()->SimpleAppendLog(Utilis::LL_INFO, msg);
		break;
	case EVENT_LOG_WARN:
		Utilis::Logger::GetInstance()->SimpleAppendLog(Utilis::LL_WARN, msg);
		break;
	case EVENT_LOG_ERR:
		Utilis::Logger::GetInstance()->SimpleAppendLog(Utilis::LL_ERROR, msg);
		break;
	default:
		Utilis::LogFatal("evLog callback meet unexpected severity(log level)\n");
		throw std::runtime_error("Event base dispatch with unexpect error code!");
		break;
	}
}

EvHttpServ::EvHttpServ(std::string const &strAddr, std::uint16_t nPort) throw (EvHttpServRTEXCP){
    if(strAddr != ""){
        unsigned long uAddr = inet_addr(strAddr.c_str());
        if( INADDR_NONE == uAddr ){
            Utilis::LogFatal("Server address illegal, inet_addr convertion failed!\n");
            throw EvHttpServRTEXCP("Server address illegal，inet_addr convertion failed!");
        }
        servAddr_ = strAddr;
    }
    if(nPort > 0){  servPort_ = nPort; }

    evBase_ = event_base_new();
    if(!evBase_){
        Utilis::LogFatal("Event base initialize failed!\n");
        throw EvHttpServRTEXCP("Event base initialize failed!");
    }
    evHttp_ = evhttp_new(evBase_);
    if(!evHttp_){
        Utilis::LogFatal("Event http initialize failed!\n");
        throw EvHttpServRTEXCP("Event http initialize failed!");
    }
    int ret = evhttp_bind_socket(evHttp_, servAddr_.c_str(),servPort_);
    if(ret != 0){
        Utilis::LogFatal("Http bind server addr:%s & port:%d failed!\n",servAddr_.c_str(),servPort_);
        throw EvHttpServRTEXCP("Http bind server addr & port failed!");
    }

	event_set_log_callback(evLogCB);
}

void EvHttpServ::SetTimeOut(int seconds){
    evhttp_set_timeout(evHttp_, seconds);
}

void EvHttpServ::SetAllowedMethod(HttpMethodsSet methods) {
	evhttp_set_allowed_methods(evHttp_, methods);
}

void EvHttpServ::SetMaxHeaderSize(size_t num) {
	evhttp_set_max_headers_size(evHttp_, num);
}

void EvHttpServ::SetMaxBodySize(size_t num) {
	evhttp_set_max_body_size(evHttp_, num);
}

bool EvHttpServ::RegistHandler(std::string const &strUrl, HandlerFunc func){
    if(!func){  return false; }
     
    typedef  void (*handle_t)(EvHttpResp *);

    //#TODO add middleware support
    auto TransFunc = [] (struct evhttp_request *req, void *arg) {
        if(nullptr == req){
            Utilis::LogWarn("Evhttp Request handler is NULL\n");
            return;
        }
        EvHttpResp httpReq(req);
        try{
            handle_t f=reinterpret_cast<handle_t>(arg);
            f(&httpReq);
        }catch(EvHttpRespRTEXCP rtExcp){
            if(nullptr != req){  /// Judge to prevent req has already been destroied
                httpReq.RespError(500,"Server Internal Error!");
				Utilis::LogError("Evhttp Request handle with exception %d:%s\n", rtExcp.GetCode(), rtExcp.what());
            }
        }catch(std::exception e){
			Utilis::LogError("Evhttp Request handle with unexpected exception :%s\n", e.what());
        }
    };
    handle_t* pph = func.target<handle_t>();
    if(pph != nullptr ){
        /// O SUCCESS,-1 ALREADY_EXIST,-2 FAILURE
        return (-2 != evhttp_set_cb(evHttp_, strUrl.c_str(), TransFunc,reinterpret_cast<void*>(*pph)));
    }else{
        return false; 
    }
}

bool EvHttpServ::UnRegistHandler(std::string const &strUrl){
    return (0 == evhttp_del_cb(evHttp_, strUrl.c_str()));
}

bool EvHttpServ::Start() throw(EvHttpServRTEXCP) {
    int ret = event_base_dispatch(evBase_);
    if(0 == ret) return true;
    else if(1 == ret){
		Utilis::LogFatal("Event base dispatch failed with no events pending or active!\n");
		return false;
    }else if(-1 == ret){
		Utilis::LogFatal("Event base dispatch failed with error occurred!\n");
		return false;
    }else {
        Utilis::LogFatal("Event base dispatch with unexpect error code!\n");
        throw EvHttpServRTEXCP("Event base dispatch with unexpect error code!");
    }
}

}//!namespace Network

