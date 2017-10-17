#include "LDLogMsg.h"

bool MedctrlDebug::m_iFileFlag = true;
QMutex MedctrlDebug::mutex;

LDLogMsg::LDLogMsg(LogMsgType type, const char* file, int line) : logFile(LOG_FILE)
{
	if (LDLogMsg::m_iFileFlag)
	{
		if (!logFile.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			return;
		}

		mutex.lock();
		if (LOG_SIZE < logFile.size())
		{
			system("rm -rf /medctrl.log.bak");
			logFile.copy(LOG_FILE_BAK);
			logFile.resize(0);
		}

		stream = new Stream(type, &logFile);
	}
	else
	{
		stream = new Stream(type);
	}

	QString text = QString("[%1] ").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz"));
	switch(type)
	{
	case DebugMsg:
		text += QString("[Debug:    ]");
		break;

	case WarningMsg:
		text += QString("[Warning:  ]");
		break;

	case CriticalMsg:
		text += QString("[Info:     ]");
		break;

	case FatalMsg:
		text += QString("[Error:    ]");
		break;
	default:
		text += QString("[Sys:      ]");
		break;
	}

	QString strName = QString("%1").arg(file);
	QStringList nameList = strName.split("/");
	strName.clear();

	strName = nameList.last();

	text += QString(" [%1] [%2] ").arg(strName, FILE_NAME_LEN).arg(line, FILE_LINE_LEN);

	stream->ts << text;
}


LDLogMsg::~LDLogMsg() {
	if (!--stream->ref) {
		if(stream->message_output) {
			QT_TRY {
				qt_message_output((QtMsgType)stream->type, stream->buffer.toLocal8Bit().data());
			} QT_CATCH(std::bad_alloc&) { /* We're out of memory - give up. */ }
		}

		mutex.unlock();
		stream->ts << endl;
		if (LDLogMsg::m_iFileFlag)
		{
			logFile.flush();
			logFile.close();
		}
    
		delete stream;
	}
}
