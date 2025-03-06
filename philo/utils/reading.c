/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   reading.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:38:30 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:38:32 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philosophers.h"

size_t	get_main_time(t_philo *philo)
{
	size_t	cur;

	pthread_mutex_lock(&philo->main->synchro);
	cur = philo->main->cur;
	pthread_mutex_unlock(&philo->main->synchro);
	return (cur);
}

int	read_meals(t_philo *philo)
{
	int	cur;

	pthread_mutex_lock(&philo->phil_mtx);
	cur = philo->meals;
	pthread_mutex_unlock(&philo->phil_mtx);
	return (cur);
}

int	read_even(t_philo *philo)
{
	int	val;

	pthread_mutex_lock(&philo->main->even_mtx);
	val = philo->main->even;
	pthread_mutex_unlock(&philo->main->even_mtx);
	return (val);
}

int	read_died(t_philo *philo)
{
	int	died;

	pthread_mutex_lock(&(philo->main->died_mtx));
	died = philo->main->died;
	pthread_mutex_unlock(&(philo->main->died_mtx));
	return (died);
}

size_t	read_last(t_philo *philo)
{
	size_t	last;

	pthread_mutex_lock(&(philo->phil_mtx));
	last = philo->last_meal;
	pthread_mutex_unlock(&(philo->phil_mtx));
	return (last);
}
