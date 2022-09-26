#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/uio.h>

void timediff( struct timespec * t1, struct timespec * t2, struct timespec *tdiff ){
    tdiff->tv_sec = t2->tv_sec - t1->tv_sec;
    tdiff->tv_nsec = t2->tv_nsec - t1->tv_nsec;

    if( t2->tv_nsec < t1->tv_nsec ){
        tdiff->tv_sec--;
        tdiff->tv_nsec += 1e+9;
    }
}

int getval(void) {
    static int i=0;
    
    return i++;
}

void get_routine_latency( void *f, const char * s ){
    struct timespec tstart, tend, tdiff;
    int i;
    double total_nsecs;
    int iters=100000;
    int(*func)(void) = f;
    
    clock_gettime( CLOCK_REALTIME, &tstart);
    for(int idx=0; idx<iters; idx++ ){
        i = func();
    }
    clock_gettime( CLOCK_REALTIME, &tend);
    timediff( &tstart, &tend, &tdiff );
    total_nsecs = tdiff.tv_sec*1e+9 + tdiff.tv_nsec;
    printf("%s latency: %.2lf nsecs\n", s, total_nsecs/iters );
}

int main(){
    
    get_routine_latency( getval, "getval()" );
    get_routine_latency( close, "close()" );

    return 0;
}
