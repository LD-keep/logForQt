#ifndef LDLOGMSG_H
#define LDLOGMSG_H

#include <QtCore/qtextstream.h>
#include <QtCore/qcontiguouscache.h>
#include <QFile>
#include <QDateTime>
#include <QStringList>
#include <QMutex>

#define LOG_FILE            ("/logMsg.log")
#define LOG_FILE_BAK        ("/logMsg.log.bak")
#define FILE_NAME_LEN       (32)
#define FILE_LINE_LEN       (5)

#define LOG_SIZE            (1024*1024*30) //30M

enum LogMsgType { DebugMsg, WarningMsg, CriticalMsg, FatalMsg, SystemMsg = QtCriticalMsg};

class  LDLogMsg
{
    struct Stream {
        Stream(QIODevice *device) : ts(device), ref(1), type(DebugMsg), space(true), message_output(false) {}
        Stream(QString *string) : ts(string, QIODevice::WriteOnly), ref(1), type(DebugMsg), space(true), message_output(false) {}
        Stream(LogMsgType t, QIODevice *io) : ts(io), ref(1), type(t), space(true), message_output(true) {}
        Stream(LogMsgType t) : ts(&buffer, QIODevice::WriteOnly), ref(1), type(t), space(true), message_output(true) {}
        QTextStream ts;
        QString buffer;
        int ref;
        LogMsgType type;
        bool space;
        bool message_output;
    } *stream;
public:

    QFile logFile;
    static QMutex mutex;

    /****
     *
     * file log mode : true output to the file,
     *                 false output to the console
     *                 default is ture
     *
     * ***/

    static bool m_iFileFlag;

    static void setFileFlag(bool flag) { m_iFileFlag = flag;}

    inline LDLogMsg(QIODevice *device) : stream(new Stream(device)) {}
    inline LDLogMsg(QString *string) : stream(new Stream(string)) {}
    inline LDLogMsg(LogMsgType type, const char* file, int line) : logFile(LOG_FILE)
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
    inline LDLogMsg(const LDLogMsg &o):stream(o.stream) { ++stream->ref; }
    inline LDLogMsg &operator=(const LDLogMsg &other);
    inline ~LDLogMsg() {
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
    inline LDLogMsg &space() { stream->space = true; stream->ts << ' '; return *this; }
    inline LDLogMsg &nospace() { stream->space = false; return *this; }
    inline LDLogMsg &maybeSpace() { if (stream->space) stream->ts << ' '; return *this; }

    inline LDLogMsg &operator<<(QChar t) { stream->ts << '\'' << t << '\''; return maybeSpace(); }
    inline LDLogMsg &operator<<(QBool t) { stream->ts << (bool(t != 0) ? "true" : "false"); return maybeSpace(); }
    inline LDLogMsg &operator<<(bool t) { stream->ts << (t ? "true" : "false"); return maybeSpace(); }
    inline LDLogMsg &operator<<(char t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(signed short t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(unsigned short t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(signed int t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(unsigned int t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(signed long t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(unsigned long t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(qint64 t)
        { stream->ts << QString::number(t); return maybeSpace(); }
    inline LDLogMsg &operator<<(quint64 t)
        { stream->ts << QString::number(t); return maybeSpace(); }
    inline LDLogMsg &operator<<(float t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(double t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(const char* t) { stream->ts << QString::fromAscii(t); return maybeSpace(); }
    inline LDLogMsg &operator<<(const QString & t) { stream->ts << '\"' << t  << '\"'; return maybeSpace(); }
    inline LDLogMsg &operator<<(const QStringRef & t) { return operator<<(t.toString()); }
    inline LDLogMsg &operator<<(const QLatin1String &t) { stream->ts << '\"'  << t.latin1() << '\"'; return maybeSpace(); }
    inline LDLogMsg &operator<<(const QByteArray & t) { stream->ts  << '\"' << t << '\"'; return maybeSpace(); }
    inline LDLogMsg &operator<<(const void * t) { stream->ts << t; return maybeSpace(); }
    inline LDLogMsg &operator<<(QTextStreamFunction f) {
        stream->ts << f;
        return *this;
    }

    inline LDLogMsg &operator<<(QTextStreamManipulator m)
    { stream->ts << m; return *this; }
};

#define LOG_DEBUG LDLogMsg(DebugMsg, __FILE__, __LINE__)
#define LOG_INFO LDLogMsg(CriticalMsg, __FILE__, __LINE__)
#define LOG_WARNING LDLogMsg(WarningMsg, __FILE__, __LINE__)
//#define LOG_ERROR LDLogMsg(FatalMsg, __FILE__, __LINE__)
#define LOG_SYSTEM LDLogMsg(SystemMsg, __FILE__, __LINE__)

#endif // LDLOGMSG_H
