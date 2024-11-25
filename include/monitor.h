#ifndef _MONITOR_H
#define _MONITOR_H

void start_monitor(int* background_processes, int* monitor);
void stop_monitor(int* background_processes, int* monitor);
void status_monitor(int* background_processes, int* monitor);

#endif