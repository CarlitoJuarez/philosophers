/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:42:28 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:42:28 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philosophers.h"

void	set_up(t_philo *philo)
{
	pthread_mutex_lock(&(philo->main->start));
	pthread_mutex_unlock(&(philo->main->start));
	set_start(philo);
}

void	start_utils_1(t_philo *philo)
{
	eat(philo);
	if (philo->id == philo->main->num_philos
		|| philo->id == philo->main->num_philos - 1)
		set_even(philo, 0);
}

void	start_utils_2(t_philo *philo)
{
	eat(philo);
	if (philo->id == philo->main->num_philos - 1
		|| philo->id == philo->main->num_philos - 2)
	{
		if (philo->main->num_philos % 2 == 0)
			set_even(philo, 1);
		else
			set_even(philo, 2);
	}
}
