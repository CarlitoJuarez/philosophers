/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:30:34 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:30:35 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philosophers.h"

void	*start(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	set_up(philo);
	while (!read_died(philo) && read_meals(philo))
	{
		if ((!philo->eaten && philo->id % 2 == 0 && read_even(philo) == 1)
			|| philo->main->num_philos == 1)
			start_utils_1(philo);
		else if (!philo->eaten && philo->id % 2 != 0
			&& read_even(philo) == 0 && ((philo->id != philo->main->num_philos
					&& philo->main->num_philos % 2 != 0)
				|| philo->main->num_philos % 2 == 0))
			start_utils_2(philo);
		else if (read_even(philo) == 2 && !philo->eaten
			&& philo->id == philo->main->num_philos
			&& philo->main->num_philos % 2 != 0 && eat(philo))
			set_even(philo, 1);
		if (philo->main->num_philos == 1)
			return (NULL);
		if (philo->eaten && !read_died(philo) && sleeping(philo))
			thinking(philo);
		usleep(1000);
	}
	return (NULL);
}

int	init_part_two(t_main **main)
{
	int	i;

	i = -1;
	pthread_mutex_lock(&(*main)->start);
	while (++i < (*main)->num_philos)
	{
		set_start(&(*main)->philos[i]);
		if (pthread_create(&(*main)->philos[i].thread_id, NULL,
				start, &(*main)->philos[i]) != 0)
			return (pthread_mutex_unlock(&(*main)->start),
				printf("Error: pthread_create\n"), 1);
	}
	pthread_mutex_unlock(&(*main)->start);
	return (0);
}

int	init_part_one(t_main **main, int i)
{
	while (++i < (*main)->num_philos)
	{
		(*main)->forks[i].fork_id = i;
		if (pthread_mutex_init(&(*main)->forks[i].fork, NULL) != 0)
			return (printf("Error: pthread_mutex_init\n"), 1);
		if (pthread_mutex_init(&(*main)->philos[i].phil_mtx, NULL) != 0)
			return (printf("Error: pthread_mutex_init\n"), 1);
	}
	i = -1;
	while (++i < (*main)->num_philos)
	{
		(*main)->tresh = 1;
		(*main)->philos[i].id = i + 1;
		(*main)->philos[i].eaten = 0;
		(*main)->philos[i].right = &(*main)->forks[i];
		if (i == (*main)->num_philos - 1)
			(*main)->philos[i].left = &(*main)->forks[0];
		else
			(*main)->philos[i].left = &(*main)->forks[i + 1];
		(*main)->philos[i].meals = (*main)->meals;
		(*main)->philos[i].main = *main;
	}
	calc_tresh(*main, 0);
	return (0);
}

int	init_mutexes(t_main *main)
{
	if (pthread_mutex_init(&main->synchro, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->meals_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->died_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->even_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->start, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->out, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	return (0);
}

int	init(t_main *main, long long *argv)
{
	main->philos = malloc(sizeof(t_philo) * (argv[0]));
	if (!main->philos)
		return (1);
	main->forks = malloc(sizeof(t_fork) * (argv[0]));
	if (!main->forks)
		return (1);
	main->num_philos = (int)argv[0];
	main->die = (size_t)argv[1];
	main->eat = (size_t)argv[2];
	main->sleep = (size_t)argv[3];
	main->meals = (size_t)argv[4];
	main->even = 1;
	main->died = 0;
	if (init_mutexes(main) != 0)
		return (1);
	if (init_part_one(&main, -1) != 0)
		return (1);
	if (init_part_two(&main) != 0)
		return (1);
	return (0);
}
