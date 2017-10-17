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
 
 
5. config
#define LOG_FILE            ("/logMsg.log") //log file path
#define LOG_FILE_BAK        ("/logMsg.log.bak")//back up file 
#define FILE_NAME_LEN       (32) //file name len
#define FILE_LINE_LEN       (5) // file line number len

#define LOG_SIZE            (1024*1024*30) //30M file size
