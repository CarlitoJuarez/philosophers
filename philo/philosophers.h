/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:56:13 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:56:15 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <limits.h>
# include <pthread.h>
# include <sys/wait.h>
# include <sys/time.h>

// MAIN

typedef struct s_main		t_main;

// MUTEX

typedef pthread_mutex_t		t_mtx;

// TIME

typedef struct timeval		t_time;

// FORK

typedef struct s_fork
{
	t_mtx	fork;
	int		fork_id;
}	t_fork;

// PHILOSOPHER

typedef struct s_philo
{
	int			id;

	pthread_t	thread_id;

	t_mtx		phil_mtx;

	t_fork		*left;
	t_fork		*right;

	size_t		last_meal;
	size_t		meals;

	int			eaten;

	t_main		*main;

}	t_philo;

typedef struct s_main
{
	t_philo		*philos;
	t_fork		*forks;

	t_mtx		synchro;
	t_mtx		even_mtx;
	t_mtx		died_mtx;
	t_mtx		meals_mtx;
	t_mtx		start;
	t_mtx		out;

	int			num_philos;
	int			tresh;
	int			even;
	int			died;

	size_t		die;
	size_t		eat;
	size_t		sleep;
	size_t		meals;
	size_t		cur;

}	t_main;

//  init.c

int			init(t_main *main, long long *argv);

//	reading.c

size_t		get_main_time(t_philo *philo);
size_t		read_last(t_philo *philo);

int			read_meals(t_philo *philo);
int			read_even(t_philo *philo);
int			read_died(t_philo *philo);

//	setting.c

void		minus_meals(t_philo *philo);
void		set_start(t_philo *philo);
void		calc_tresh(t_main *main, int tresh_2);
void		set_even(t_philo *philo, int val);

int			set_time(t_philo *philo);

//	simulate.c

int			eat(t_philo *philo);
int			thinking(t_philo *philo);
int			sleeping(t_philo *philo);

//  start_utils.c

void		start_utils_2(t_philo *philo);
void		start_utils_1(t_philo *philo);
void		set_up(t_philo *philo);

//	utils_1.c

void		custom_usleep(long long time_in_ms, t_philo *philo);
void		print_timestamp_fork(t_philo *philo);

int			check_in(char **arr);

long long	ft_atoi(char *s, int i, int count, int sign);

#endif
