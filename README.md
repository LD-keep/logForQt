# logForQt
1. LDLogMsg Based on Qt prepared to support the output log to the file, output to the control station
     //default open file log mode, false -- Output to the console
    LDLogMsg::setFileFlag(false);

2. demo
  #include "LDLogMsg.h"

int main(int argc,char* argv[])
{
    LOG_DEBUG << "Hello,LOG!";
}

[2017-10-17 09:41:40:741] [Debug:    ] [                     main.cpp] [ 1390 ] Hello,LOG!


3. log level
  DebugMsg, WarningMsg, CriticalMsg, FatalMsg, SystemMsg = CriticalMsg
  
4.Log type function
 LOG_DEBUG
 LOG_INFO
 LOG_WARNING
 LOG_SYSTEM
 LOG_ERROR //Not currently supported
