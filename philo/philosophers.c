/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:22:01 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:22:03 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

int	time_up(t_philo *philo)
{
	t_time	cur;
	size_t	main;

	main = get_main_time(philo);
	gettimeofday(&cur, NULL);
	pthread_mutex_lock(&(philo->main->out));
	if (((size_t)cur.tv_sec * 1000 + (size_t)cur.tv_usec / 1000) - main
		> read_last(philo) - main
		+ philo->main->die + philo->main->tresh)
		return (pthread_mutex_unlock(&(philo->main->out)), 1);
	pthread_mutex_unlock(&(philo->main->out));
	return (0);
}

int	check_died(t_main *main, int i, int count)
{
	int		cur_meals;
	t_time	cur;

	count = main->num_philos;
	while (++i < main->num_philos)
	{
		cur_meals = read_meals(&main->philos[i]);
		if (!cur_meals)
			count--;
		if (!count)
			return (1);
		if (time_up(&main->philos[i]))
		{
			if (main->tresh == -2)
				usleep(1800);
			gettimeofday(&cur, NULL);
			pthread_mutex_lock(&main->out);
			printf("%ld: %d died\n", ((size_t)cur.tv_sec * 1000
					+ (size_t)cur.tv_usec / 1000)
				- get_main_time(&main->philos[0]), main->philos[i].id);
			pthread_mutex_unlock(&main->out);
			return (1);
		}
	}
	return (0);
}

int	destroy_mtx(t_main *main)
{
	if (pthread_mutex_destroy(&main->synchro) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->died_mtx) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->meals_mtx) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->even_mtx) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->start) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->out) != 0)
		return (1);
	return (0);
}

int	run_the_rest(t_main *main, int i)
{
	pthread_mutex_lock(&main->start);
	pthread_mutex_unlock(&main->start);
	while (!(check_died(main, -1, 0)))
		usleep(1000);
	pthread_mutex_lock(&main->died_mtx);
	main->died = 1;
	pthread_mutex_unlock(&main->died_mtx);
	while (++i < main->num_philos)
	{
		if (pthread_join(main->philos[i].thread_id, NULL) != 0)
			return (printf("Error: pthread_join\n"), 1);
	}
	i = -1;
	while (++i < main->num_philos)
	{
		if (pthread_mutex_destroy(&main->philos[i].left->fork) != 0)
			return (1);
		if (pthread_mutex_destroy(&main->philos[i].phil_mtx) != 0)
			return (1);
	}
	free(main->philos);
	free(main->forks);
	return (destroy_mtx(main));
}

int	main(int argc, char **argv)
{
	t_main			main;
	long long		arr[6];
	int				i;

	i = -1;
	arr[5] = 0;
	if (argc == 5 || argc == 6)
	{
		if (!check_in(argv + 1))
			return (printf("Invalid input!\n"), 0);
		while (argv[++i + 1])
			arr[i] = ft_atoi(argv[i + 1], 0, 0, 1);
		if (argc == 5)
			arr[i] = -1;
		if (init(&main, arr) != 0)
			return (1);
		if (run_the_rest(&main, -1) != 0)
			return (1);
	}
	else
		return (printf("Invalid argc!\n"), 0);
	return (0);
}
