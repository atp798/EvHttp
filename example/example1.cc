/**
 * Copyright (c) 2019, ATP
 * All rights reserved.
 * MIT License
 */

#include "EvHttpServ.h"
#include "Utilis/Defer.h"
#include "Utilis/Logger.h"

using namespace Network;

int main()
{
  EvHttpServ Serv("0.0.0.0", 8077);

  Serv.RegistHandler("/hi/testget",  [](EvHttpResp *resp){
      resp->QuickResponse(200,"Hello World!\n");});

  Serv.Start();
  return 0;
}