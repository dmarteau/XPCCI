#ifndef dmTimer_h
#define dmTimer_h

#include <time.h>

class dmTimer
{
    protected:
     clock_t t_start;
     clock_t t_finish;

    public:
     dmTimer() { Start(); }

     void Start()   { t_finish = t_start = clock(); }
     void Stop()    { t_finish = clock();  }

     double Duration() const { return static_cast<double>(t_finish - t_start)  / CLOCKS_PER_SEC; }
     double Elapsed()  const { return static_cast<double>(clock()  - t_start)  / CLOCKS_PER_SEC; }
     double Delta()    const { return static_cast<double>(clock()  - t_finish) / CLOCKS_PER_SEC; }

     void Wait( double time_to_wait ) {
        t_start  = clock();
        t_finish = static_cast<clock_t>(time_to_wait*CLOCKS_PER_SEC) + t_start;
        while( t_finish > clock() );
     }

     bool IsElapsed( double elapsed_time, bool restart_if_elapsed ) {
        if( Elapsed() >= elapsed_time ) {
           if(restart_if_elapsed) Start();
           return true;
        }
        return false;
     }
};

#endif // dmTimer_h
