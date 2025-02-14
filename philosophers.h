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
    t_mtx       phil_mtx;

    t_fork        *left;
    t_fork        *right;

    size_t         last_meal;
    size_t         eaten;
    size_t         died;
    int         meals;

    main_info   *main;
    
} t_philo;

typedef struct main_info
{
    t_philo   *philos;
    t_fork    *forks;

    t_mtx   synchro;
    t_mtx   even_mtx;
    t_mtx   died_mtx;
    t_mtx   meals_mtx;

    int     died;
    int     even;

    int     tresh;

    int     num_philos;
    int     die;
    int     eat;
    int     sleep;
    int     meals;
    t_time      cur;

} main_info;

// Utils

long ft_atoi(char *s);

int	read_meals(t_philo *philo);

int	read_died(t_philo *philo);

int	read_even(t_philo *philo);

int	time_up(t_philo *philo);


#endif