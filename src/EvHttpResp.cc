/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#include "EvHttpResp.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <functional>

#include "event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/bufferevent_compat.h"
#include "event2/http.h"
#include "event2/http_compat.h"
#include "event2/http_struct.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "evhttp.h"

#include "Utilis/Defer.h"
#include "Utilis/Logger.h"

namespace Network {

EvHttpResp::EvHttpResp(struct evhttp_request *req) throw(EvHttpRespRTEXCP)
    : evReq_(req) {
  if (nullptr == evReq_) {
    Utilis::LogError("Request is Null\n");
    throw EvHttpRespRTEXCP(500, "Request is Null");
  }
  evUri_ = evhttp_request_get_evhttp_uri(evReq_);
  if (evUri_) {
    const char *query = evhttp_uri_get_query(evUri_);
    if (query) {
      evhttp_parse_query_str(query, &pathParams_);
    }
  }
  headParams_ = evhttp_request_get_input_headers(evReq_);

  respHeaders_ = evhttp_request_get_output_headers(evReq_);
  respBuf_ = evhttp_request_get_output_buffer(evReq_);
}

EvHttpResp::~EvHttpResp() {}

std::string EvHttpResp::GetHeadParam(std::string const &strKey) {
  if (headParams_) {
    const char *val = evhttp_find_header(headParams_, strKey.c_str());
    if (val) {
      return std::string(val);
    } /// Must judge herer to prevent val be NULL
  }
  return std::string("");
}

std::string EvHttpResp::GetPathParam(std::string const &strKey) {
  const char *val = evhttp_find_header(&pathParams_, strKey.c_str());
  if (val) {
    return std::string(val);
  }
  return std::string("");
}

void EvHttpResp::parsePostParam() {
  // if(postParamParsed){ return; }/// Rejudge here to prevent misuse
  postParamParsed = true;
  size_t len = 0;
  len = evbuffer_get_length(evReq_->input_buffer);
  if (len > 0) {
    // size_t msgLen = len > MAX_POST_BODY_LEN ? MAX_POST_BODY_LEN : len;
    /// #TODO pullup may not necessary
    evhttp_parse_query_str(reinterpret_cast<const char *>(
                               evbuffer_pullup(evReq_->input_buffer, -1)),
                           &postParams_);
  }
}

std::string EvHttpResp::GetPostParam(std::string const &strKey) {
  /// Judge here is not useless, in most case, judge in the functional call is a
  /// waste
  if (!postParamParsed) {
    parsePostParam();
  }
  const char *val = evhttp_find_header(&postParams_, strKey.c_str());
  if (val) {
    return std::string(val);
  }
  return std::string("");
}

std::string EvHttpResp::GetRequestUri() {
  if (evReq_) {
    const char *uri = evhttp_request_get_uri(evReq_);
    if (uri) {
      return std::string(uri);
    }
  }
  return std::string("");
}

std::string EvHttpResp::GetUriHost() {
  if (evUri_) {
    const char *host = evhttp_uri_get_host(evUri_);
    if (host) {
      return std::string(host);
    }
  }
  return std::string("");
}

int EvHttpResp::GetUriPort() { return evhttp_uri_get_port(evUri_); }

std::string EvHttpResp::GetUriPath() {
  if (evUri_) {
    const char *path = evhttp_uri_get_path(evUri_);
    if (path) {
      return std::string(path);
    }
  }
  return std::string("");
}

std::string EvHttpResp::GetUriQuery() {
  if (evUri_) {
    const char *query = evhttp_uri_get_query(evUri_);
    if (query) {
      return std::string(query);
    }
  }
  return std::string("");
}

std::string EvHttpResp::GetUriFragment() {
  if (evUri_) {
    const char *fragment = evhttp_uri_get_fragment(evUri_);
    if (fragment) {
      return std::string(fragment);
    }
  }
  return std::string("");
}

std::string EvHttpResp::GetPostMsg() {
  if (!strBody_.empty()) {
    return strBody_;
  }
  size_t len = 0;
  len = evbuffer_get_length(evReq_->input_buffer);
  if (len > 0) {
    strBody_.resize(len);
    strBody_.assign(reinterpret_cast<const char *>(
                        evbuffer_pullup(evReq_->input_buffer, -1)),
                    len);
  }
  return strBody_;
}

bool EvHttpResp::AddRespHeadParam(std::string const &key,
                                  std::string const &val) {
  if (0 != evhttp_add_header(respHeaders_, key.c_str(), val.c_str())) {
    Utilis::LogWarn("Add parameter of response header failed\n");
    return false;
  }
  return true;
}

void EvHttpResp::AddRespHeaders(HttpHeaders &headers) {
  HttpHeaders::iterator iter;
  for (iter = headers.begin(); iter != headers.end(); iter++) {
    HttpHeadVal::iterator itval;
    HttpHeadVal vallist = iter->second;
    for (itval = vallist.begin(); itval != vallist.end(); itval++) {
      AddRespHeadParam(iter->first, *itval);
    }
  }
}

bool EvHttpResp::AddRespString(std::string const &str) {
  if (-1 == evbuffer_add_printf(respBuf_, str.c_str())) {
    Utilis::LogWarn("Add string to response body failed\n");
    return false;
  }
  return true;
}

bool EvHttpResp::AddRespBuf(void const *data, std::size_t len) {
  auto cbFreeBuf = [](const void *pData, size_t datalen, void *extra) {
    ::operator delete(const_cast<void *>(pData));
  };

  void *dataCpy = ::operator new(len);
  std::memcpy(dataCpy, data, len);
  if (0 != evbuffer_add_reference(respBuf_, dataCpy, len, cbFreeBuf, nullptr)) {
    ::operator delete(dataCpy);
    Utilis::LogWarn("Add data buffer to response body failed\n");
    return false;
  }
  return true;
}

void EvHttpResp::SetRespCode(int code) { respCode_ = code; }

bool EvHttpResp::AddRespFile(std::string const &fileName) {
  int fd = open(fileName.c_str(), 0);
  if (fd == -1) {
    Utilis::LogError(
        "Add file content to response body failed: open file error\n");
    return false;
  }
  Utilis::DEFER([&] {
    if (fd != -1)
      close(fd);
  });
  ev_off_t len = lseek(fd, 0, SEEK_END);
  if (evbuffer_add_file(respBuf_, fd, 0, len) == -1) {
    Utilis::LogError(
        "Add file content to response body failed: evbuffer_add_file error\n");
    return false;
  }
  return true;
}

void EvHttpResp::SendResponse() {
  evhttp_send_reply(evReq_, respCode_, nullptr, respBuf_);
}

void EvHttpResp::QuickResponse(int code, std::string const &strBody) {
  AddRespString(strBody);
  evhttp_send_reply(evReq_, code, nullptr, respBuf_);
}

void EvHttpResp::SimpleResponse(int code, HttpHeaders &headers,
                                std::string const &strBody) {
  AddRespHeaders(headers);
  AddRespString(strBody);
  evhttp_send_reply(evReq_, respCode_, nullptr, respBuf_);
}

void EvHttpResp::RespError(int nCode, std::string const &strMsg) {
  if (strMsg.empty()) {
    evhttp_send_error(evReq_, nCode, nullptr);
  } else {
    evhttp_send_error(evReq_, nCode, strMsg.c_str());
  }
}

} // namespace Network