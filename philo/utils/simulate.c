/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulate.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:41:41 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:41:42 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philosophers.h"

int	lock_forks(t_philo *philo)
{
	if (philo->main->num_philos == 1
		&& philo->left->fork_id == philo->right->fork_id)
	{
		pthread_mutex_lock(&philo->left->fork);
		print_timestamp_fork(philo);
		return (pthread_mutex_unlock(&philo->left->fork), 0);
	}
	set_time(philo);
	if (philo->id % 2 == 0 && !read_died(philo) && read_meals(philo))
	{
		pthread_mutex_lock(&philo->left->fork);
		print_timestamp_fork(philo);
		pthread_mutex_lock(&philo->right->fork);
		print_timestamp_fork(philo);
		return (1);
	}
	else if (!read_died(philo) && read_meals(philo))
	{
		pthread_mutex_lock(&philo->right->fork);
		print_timestamp_fork(philo);
		pthread_mutex_lock(&philo->left->fork);
		print_timestamp_fork(philo);
		return (1);
	}
	return (0);
}

int	sleeping(t_philo *philo)
{
	t_time	cur;
	t_time	timestamp;
	size_t	main;

	gettimeofday(&cur, NULL);
	main = get_main_time(philo);
	pthread_mutex_lock(&(philo->main->out));
	if (!read_died(philo))
	{
		printf("%ld: %d is sleeping\n", ((size_t)cur.tv_sec
				* 1000 + (size_t)cur.tv_usec / 1000)
			- main, philo->id);
	}
	pthread_mutex_unlock(&(philo->main->out));
	gettimeofday(&timestamp, NULL);
	while (!read_died(philo) && (size_t)cur.tv_sec * 1000
		+ (size_t)cur.tv_usec / 1000
		< (size_t)timestamp.tv_sec * 1000
		+ (size_t)timestamp.tv_usec / 1000 + philo->main->sleep)
	{
		usleep(1000);
		gettimeofday(&cur, NULL);
	}
	philo->eaten = 0;
	return (1);
}

int	thinking(t_philo *philo)
{
	t_time	cur;
	size_t	main;

	gettimeofday(&cur, NULL);
	main = get_main_time(philo);
	pthread_mutex_lock(&(philo->main->out));
	if (!read_died(philo))
	{
		printf("%ld: %d is thinking\n", ((size_t)cur.tv_sec * 1000
				+ (size_t)cur.tv_usec / 1000) - main, philo->id);
	}
	pthread_mutex_unlock(&(philo->main->out));
	return (1);
}

int	eat(t_philo *philo)
{
	t_time	cur;
	size_t	main;

	main = get_main_time(philo);
	if (read_meals(philo) && !read_died(philo) && lock_forks(philo))
	{
		gettimeofday(&cur, NULL);
		pthread_mutex_lock(&philo->main->out);
		if (!read_died(philo))
		{
			printf("%ld: %d is eating\n", ((size_t)cur.tv_sec
					* 1000 + (size_t)cur.tv_usec / 1000)
				- main, philo->id);
		}
		pthread_mutex_unlock(&philo->main->out);
		custom_usleep(philo->main->eat, philo);
		pthread_mutex_unlock(&philo->right->fork);
		pthread_mutex_unlock(&philo->left->fork);
		minus_meals(philo);
		philo->eaten = 1;
	}
	return (1);
}
