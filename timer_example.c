#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>


void SamplerHandler()
{
	printf("Handler activated\n");
}


int main()
{
	int received_sig, i = 0;
	timer_t sampling_timer;
	struct itimerspec timer_period;
	struct sigevent timer_event;
	
	// Receiver for SIGALRM signal
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_BLOCK, &set, NULL);	
	
	// Setup the handler
	timer_event.sigev_notify = SIGEV_SIGNAL;	
	timer_event.sigev_signo = SIGALRM;
	
	
	// Se crea el timer
	timer_create(CLOCK_REALTIME, &timer_event, &sampling_timer);
	
	// Se configura el periodo de disparo del timer
	timer_period.it_value.tv_sec     = 1;
	timer_period.it_value.tv_nsec    = 0;
	// Los atributos iterval hacen referencia al periodo de deisparo del timer (si son 0 solo ocurre la primera vez)
	timer_period.it_interval.tv_sec  = 1;
	timer_period.it_interval.tv_nsec = 0;
	
	// Inicia el timer
	timer_settime(sampling_timer, 0, &timer_period, NULL);	
	
	while(i < 10)
	{
		printf("i = %d\n",i);
		sigwait(&set, &received_sig);
		i++;
	}
	printf("received_sig = %d\n",received_sig);
	
	// Se destruye (deshabilita) el timer
	timer_delete(sampling_timer);
	printf("Saliendo\n");
	
	return 0;
}


