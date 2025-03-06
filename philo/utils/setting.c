/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   setting.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:40:34 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:40:35 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philosophers.h"

void	minus_meals(t_philo *philo)
{
	pthread_mutex_lock(&philo->phil_mtx);
	philo->meals--;
	pthread_mutex_unlock(&philo->phil_mtx);
}

void	set_start(t_philo *philo)
{
	t_time	cur;

	gettimeofday(&cur, NULL);
	pthread_mutex_lock(&philo->main->synchro);
	philo->main->cur = ((size_t)cur.tv_sec * 1000
			+ (size_t)cur.tv_usec / 1000);
	pthread_mutex_unlock(&philo->main->synchro);
	pthread_mutex_lock(&philo->phil_mtx);
	philo->last_meal = ((size_t)cur.tv_sec * 1000
			+ (size_t)cur.tv_usec / 1000);
	pthread_mutex_unlock(&philo->phil_mtx);
}

void	calc_tresh(t_main *main, int tresh_2)
{
	if (main->num_philos % 2 == 0)
		tresh_2 = main->die - (main->eat * 2);
	else
		tresh_2 = main->die - (main->eat * 2 + main->sleep);
	if (tresh_2 < 0)
	{
		main->tresh = 0;
		return ;
	}
	if (tresh_2 == 0)
	{
		main->tresh = -2;
		return ;
	}
	if (tresh_2 == 1 || main->num_philos < 10)
	{
		main->tresh = 5;
		return ;
	}
	while (tresh_2 > 30)
		tresh_2 /= (tresh_2 / 10);
	if (tresh_2 <= 2)
		main->tresh = 5;
	if (main->tresh < tresh_2)
		main->tresh = tresh_2;
}

int	set_time(t_philo *philo)
{
	t_time	cur;

	gettimeofday(&cur, NULL);
	pthread_mutex_lock(&philo->phil_mtx);
	philo->last_meal = ((size_t)cur.tv_sec * 1000
			+ (size_t)cur.tv_usec / 1000);
	pthread_mutex_unlock(&philo->phil_mtx);
	return (1);
}

void	set_even(t_philo *philo, int val)
{
	pthread_mutex_lock(&philo->main->even_mtx);
	philo->main->even = val;
	pthread_mutex_unlock(&philo->main->even_mtx);
}
