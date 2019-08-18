#EvHttp
### Introduction
This project is a cpp http server lib, wrapped of libevent.

It provide some basic http server features like handler regist, 
url/header/body parse, set response code/header/body and so on.

More information on [Straka's blog](http://www.straka.cn/blog/cpp-wrapped-http-server-based-on-libevent/)

Server Example
--------------

```c++
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include "EvHttpServ.h"
#include "Utilis/Defer.h"
#include "Utilis/Logger.h"
#include "Utilis/Initialize.h"

using namespace Network;

void testGetHandler(EvHttpResp *resp){
    resp->QuickResponse(200,"Hello World!\n");
}

int main()
{
	Utilis::Logger::GetInstance()->StartLogging();
	Utilis::DEFER([] { Utilis::Logger::GetInstance()->StopLogging(); });

    short http_port = 8077;
    std::string http_addr("0.0.0.0\0");
    
    EvHttpServ Serv(http_addr,http_port);
    Serv.RegistHandler("/hi/testget", testGetHandler);
    Serv.Start();
    
    return 0;
}
```

`Post`, `Put`, `Delete` and `Options` methods are NOT supported yet.

NOTE
----
It is recommended to use g++ 4.9+ to better support c++11 features.

License
-------
MIT license (© 2019 ATP)