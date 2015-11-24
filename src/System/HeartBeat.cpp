#include "HeartBeat.hpp"

void HeartBeat::SetRunning(bool running){ mRunning = running; }
void HeartBeat::SetUpdate(UpdateCb uFn){ mUpdate = uFn; }
void HeartBeat::SetRender(RenderCb rFn){ mRender = rFn; }
void HeartBeat::SetTimer(TimerFn tFn){ mTimer = tFn; }

void HeartBeat::Run()
{
    double nextUpdate = mTimer();

    const int TICKS_PER_SECOND = 25;
    const int MAX_FRAMESKIP = 5;
    const float SKIP_TICKS = 1000 / TICKS_PER_SECOND;

    int loops;
    float interpolation;

    mRunning = true;
    while (mRunning)
    {
        loops = 0;
        while (mTimer() > nextUpdate && loops < MAX_FRAMESKIP)
        {
            mUpdate(SKIP_TICKS);
            nextUpdate += SKIP_TICKS;
            ++loops;
        }

        interpolation = static_cast<float>(mTimer() + SKIP_TICKS - nextUpdate) / SKIP_TICKS;
        mRender(interpolation);
    }
}

