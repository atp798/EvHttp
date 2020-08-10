# EvHttp

Introduction
------------
This project is a cpp http server lib, wrapped of libevent.

It provides some basic http server features like handler regist, url/header/body parse, set response code/header/body and so on.

More information on [Straka's blog](http://www.straka.cn/blog/cpp-wrapped-http-server-based-on-libevent/)

Feature
-------
- Based on libevent.
- No c interface exposed.
- User friendly interface.
- Multi-platform support.

Server Example
--------------

```c++
#include "EvHttpServ.h"
#include "Utilis/Defer.h"
#include "Utilis/Logger.h"

using namespace Network;

int main()
{
    Utilis::Logger::GetInstance()->StartLogging();
    Utilis::DEFER([] { Utilis::Logger::GetInstance()->StopLogging(); });

    EvHttpServ Serv("0.0.0.0\0", 8077);
    Serv.RegistHandler("/hi/testget", [](EvHttpResp *resp){resp->QuickResponse(200,"Hello World!\n");});

    Serv.Start();
    return 0;
}
```

NOTE
----

More examples can be found in folder "example".

`Post`, `Put`, `Delete` and `Options` methods are NOT supported yet.

It is recommended to use g++ 4.9+ to support c++11 features better.

Usage
-----
1. Download: `git clone --recursive https://github.com/atp798/EvHttp.git`

2. Build: `mkdir build && cd build && cmake ../ && make`

3. Run sample: `./EvHttpExample1`

4. Test the http server: `curl -X GET x.x.x.x:8077/hi/testget`. The ip address 'x.x.x.x' should be replaced by the server ip address.

TODO list
---------
- Make it thread-safe.
- Add http client feature.
- Add tcp server & client.
- Support https.
- Support udp.

License
-------
MIT license (© 2019 ATP)
