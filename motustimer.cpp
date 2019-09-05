#include "motustimer.h"
#include <MMSystem.h>
#ifdef __MINGW32__ //w32api bug
#define TIME_KILL_SYNCHRONOUS 0x0100
#endif

//构造函数
MotusTimer::MotusTimer(int timer, QObject *parent) :QObject(parent),m_timer(timer)
{
    m_id=0;
}

//回调函数
void WINAPI CALLBACK MotusTimer::mmtimer_proc(uint timerId, uint uMsg, DWORD_PTR user, DWORD_PTR, DWORD_PTR)
{
    MotusTimer *t = reinterpret_cast<MotusTimer*>(user);
    if(timerId>uMsg)
    {}
    emit t->timeout();
}

//析构函数
MotusTimer::~MotusTimer()
{
    stop();
}

//定时器开始
void MotusTimer::start()
{
    m_id = timeSetEvent(m_timer, 1, mmtimer_proc, (DWORD_PTR)this,
        TIME_CALLBACK_FUNCTION | TIME_PERIODIC | TIME_KILL_SYNCHRONOUS);

}

//关闭定时器
void MotusTimer::stop()
{
    if (m_id)
    {
        timeKillEvent(m_id);
        m_id = 0;
    }
}
