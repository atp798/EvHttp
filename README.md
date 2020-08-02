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

TODO
----
- Reorganize project, add libevent as third_party dependancy.
- Refine code.
- Make it thread-safe.
- Add client support.

License
-------
MIT license (© 2019 ATP)
