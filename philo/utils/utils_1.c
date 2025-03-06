/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils_1.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cjuarez <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/05 16:43:29 by cjuarez           #+#    #+#             */
/*   Updated: 2025/03/05 16:43:30 by cjuarez          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philosophers.h"

long long	ft_atoi(char *s, int i, int count, int sign)
{
	size_t	res;

	res = 0;
	while (s[i] == ' ' || (s[i] >= 9 && s[i] <= 13))
		i++;
	if (s[i] == '-' || s[i] == '+')
	{
		if (s[i] == '-')
			sign = -1;
		i++;
	}
	while (s[i] == '0')
		i++;
	while (s[i] >= '0' && s[i] <= '9')
	{
		count++;
		if (res > 0 && (count > 19
				|| (count == 19 && (s[i - 1] >= '1' || s[i] >= '8'))))
			return (-1);
		res = res * 10 + (s[i++] - '0');
	}
	if (res > 0 && sign == -1)
		return (-1);
	return (res);
}

int	check_in(char **arr)
{
	int	i;

	if (!arr || !*arr)
		return (0);
	if (ft_atoi(arr[0], 0, 0, 1) > 200 || ft_atoi(arr[0], 0, 0, 1) < 1)
		return (0);
	i = 0;
	while (arr[++i])
	{
		if ((i <= 3 && ft_atoi(arr[i], 0, 0, 1) < 60)
			|| (i == 4 && ft_atoi(arr[i], 0, 0, 1) < 0))
			return (0);
	}
	return (1);
}

void	print_timestamp_fork(t_philo *philo)
{
	t_time	cur;
	size_t	main;

	main = get_main_time(philo);
	pthread_mutex_lock(&philo->main->out);
	gettimeofday(&cur, NULL);
	if (!read_died(philo))
		printf("%ld: %d has taken a fork\n", ((size_t)cur.tv_sec
				* 1000 + (size_t)cur.tv_usec / 1000)
			- main, philo->id);
	pthread_mutex_unlock(&philo->main->out);
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

void	custom_usleep(long long time_in_ms, t_philo *philo)
{
	long long	start_time;

	start_time = get_time_in_ms();
	while (!read_died(philo)
		&& (get_time_in_ms() - start_time) < time_in_ms)
		usleep(1000);
}
