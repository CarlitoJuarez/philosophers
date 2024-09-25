#ifndef PHILOSOPHERS_H
# define    PHILOSOPHERS_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <pthread.h>

#include <sys/wait.h>
#include <sys/time.h>

// MAIN

typedef struct main_info main_info;

// MUTEX

typedef pthread_mutex_t t_mtx;

// TIME

typedef struct timeval t_time;

// FORK

typedef struct s_fork
{
    t_mtx   fork;
    int     fork_id;
} t_fork;

// PHILOSOPHER

typedef struct s_philo
{
    int         id;
    pthread_t   thread_id;

    t_fork        *left;
    t_fork        *right;

    int         last_meal;
    int         eaten;
    int         died;
    int         meals;

    main_info   *main;
    
} t_philo;

typedef struct main_info
{
    t_philo   *philos;
    t_fork    *forks;

    t_mtx   synchro;
    t_mtx   synchro1;

    int     died;
    int     even;

    int     num_philos;
    int     die;
    int     eat;
    int     sleep;
    int     meals;

} main_info;

// Utils

long ft_atoi(char *s);

#endif