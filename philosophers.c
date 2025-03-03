#include "philosophers.h"

int read_meals(t_philo *philo)
{
	int cur;

	pthread_mutex_lock(&philo->phil_mtx);
	cur = philo->meals;
	pthread_mutex_unlock(&philo->phil_mtx);
	return (cur);
}

void minus_meals(t_philo *philo)
{
	pthread_mutex_lock(&philo->phil_mtx);
	philo->meals--;
	pthread_mutex_unlock(&philo->phil_mtx);
}

int	check_in(char **arr)
{
	long	i;

	if (!arr || !*arr)
		return (0);
	if (ft_atoi(arr[0]) > 200 || ft_atoi(arr[0]) < 1)
		return (0);
	i = 0;
	while (arr[++i])
	{
		if (ft_atoi(arr[i]) < 60 && ft_atoi(arr[i]) > INT_MAX && i > 0)
			return (0);
	}
	return (1);
}

int set_time(t_philo *philo)
{
	t_time cur;

	gettimeofday(&cur, NULL);
	pthread_mutex_lock(&philo->main->synchro);
	philo->last_meal = ((size_t)cur.tv_sec * 1000
		+ (size_t)cur.tv_usec / 1000);
	pthread_mutex_unlock(&philo->main->synchro);
	// printf("SET_TIME %d: %ld\n", philo->id, philo->last_meal - ((size_t)(philo->main->cur).tv_sec * 1000
	// 			+ (size_t)(philo->main->cur).tv_usec / 1000));
	return (1);
}

void set_start(t_philo *philo)
{
	pthread_mutex_lock(&philo->main->synchro);
	philo->last_meal = ((size_t)philo->main->cur.tv_sec * 1000
		+ (size_t)philo->main->cur.tv_usec / 1000);
	pthread_mutex_unlock(&philo->main->synchro);
	// printf("SET_TIME %d: %ld\n", philo->id, philo->last_meal - ((size_t)(philo->main->cur).tv_sec * 1000
	// 			+ (size_t)(philo->main->cur).tv_usec / 1000));
}

void	print_timestamp_fork(t_philo *philo)
{
	t_time	cur;

	gettimeofday(&cur, NULL);
	pthread_mutex_lock(&philo->main->synchro);
	printf("%ld: %d has taken a fork\n", ((size_t)cur.tv_sec
		* 1000 + (size_t)cur.tv_usec / 1000)
			- ((size_t)(philo->main->cur).tv_sec * 1000
				+ (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
	pthread_mutex_unlock(&philo->main->synchro);
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}


void calc_tresh(main_info *main)
{
	int tresh_2;

	// if (main->num_philos % 2 == 0)
	// 	return ;
	tresh_2 = main->die - (main->eat * 2 + main->sleep);
	if (tresh_2 < 0)
		tresh_2 = 1;
	// if (tresh_2 > 40)
	// 	tresh_2 /= (tresh_2 / 10);
	if (main->num_philos < 10)
		main->tresh = 1;
	// if (main->tresh < tresh_2)
		// main->tresh = tresh_2;
	printf("TRESH2: %d\n", tresh_2);
	if (main->tresh < tresh_2)
			main->tresh = tresh_2;
	printf("TRESH: %d\n", main->tresh);
}

int possible(t_philo *philo)
{
	if (philo->main->num_philos % 2 == 0 && philo->main->die > philo->main->eat + philo->main->sleep)
		return (usleep(5000 * philo->main->tresh), 1);
	if (philo->main->die == philo->main->eat * 2 + philo->main->sleep)
		return (0);
	if (philo->main->die > philo->main->eat * 2 + philo->main->sleep - philo->main->tresh)
		return (usleep(2000 * philo->main->tresh), 1);
	return (0);
}

int lock_forks(t_philo *philo)
{
	pthread_mutex_lock(&philo->main->synchro);
	printf("WAITING FOR LOCK: %d\n", philo->id);
	pthread_mutex_unlock(&philo->main->synchro);
	if (philo->left->fork_id == philo->right->fork_id)
	{
		pthread_mutex_lock(&philo->left->fork);
		print_timestamp_fork(philo);
		return (pthread_mutex_unlock(&philo->left->fork), 0);
	}
	if (philo->id % 2 == 0 && !read_died(philo) && read_meals(philo))
	{
		pthread_mutex_lock(&philo->left->fork);
		print_timestamp_fork(philo);
		pthread_mutex_lock(&philo->main->synchro);
		printf("LOCK: %d FROM: %d\n", philo->left->fork_id, philo->id);
		pthread_mutex_unlock(&philo->main->synchro);
		pthread_mutex_lock(&philo->right->fork);
		print_timestamp_fork(philo);
		pthread_mutex_lock(&philo->main->synchro);
		printf("LOCK: %d FROM: %d\n", philo->right->fork_id, philo->id);
		pthread_mutex_unlock(&philo->main->synchro);
	}
	else if (!read_died(philo) && read_meals(philo))
	{
		pthread_mutex_lock(&philo->right->fork);
		print_timestamp_fork(philo);
		pthread_mutex_lock(&philo->main->synchro);
		printf("LOCK: %d FROM: %d\n", philo->right->fork_id, philo->id);
		pthread_mutex_unlock(&philo->main->synchro);
		pthread_mutex_lock(&philo->left->fork);
		print_timestamp_fork(philo);
		pthread_mutex_lock(&philo->main->synchro);
		printf("LOCK: %d FROM: %d\n", philo->left->fork_id, philo->id);
		pthread_mutex_unlock(&philo->main->synchro);
	}
	return (1);
}

void	custom_usleep(long long time_in_ms,t_philo *philo)
{
	long long	start_time;

	start_time = get_time_in_ms();
	while (!read_died(philo)
		&& (get_time_in_ms() - start_time) < time_in_ms)
		usleep(100);
}

int read_even(t_philo *philo)
{
	int val;

	pthread_mutex_lock(&philo->main->even_mtx);
	val = philo->main->even;
	pthread_mutex_unlock(&philo->main->even_mtx);
	return (val);
}

void set_even(t_philo *philo, int val)
{
	pthread_mutex_lock(&philo->main->even_mtx);
	philo->main->even = val;
	pthread_mutex_unlock(&philo->main->even_mtx);
}

void	eat(t_philo *philo)
{
	t_time	cur;

	if (read_meals(philo) && !read_died(philo))
	{
		minus_meals(philo);
		lock_forks(philo);
		gettimeofday(&cur, NULL);
		set_time(philo);
		pthread_mutex_lock(&philo->main->synchro);
		if (!read_died(philo))
		{
			printf("%ld: %d is eating\n", ((size_t)cur.tv_sec
				* 1000 + (size_t)cur.tv_usec / 1000)
					- ((size_t)(philo->main->cur).tv_sec * 1000
						+ (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
		}
		pthread_mutex_unlock(&philo->main->synchro);
		// custom_usleep(philo->main->eat, philo);
		usleep(philo->main->eat * 1000);
		if (philo->id % 2 == 0)
		{
			pthread_mutex_unlock(&philo->right->fork);
			pthread_mutex_lock(&philo->main->synchro);
			printf("UNLOCK: %d FROM: %d\n", philo->left->fork_id, philo->id);
			pthread_mutex_unlock(&philo->main->synchro);
			pthread_mutex_unlock(&philo->left->fork);
			pthread_mutex_lock(&philo->main->synchro);
			printf("UNLOCK: %d FROM: %d\n", philo->right->fork_id, philo->id);
			pthread_mutex_unlock(&philo->main->synchro);
		}
		else
		{
			pthread_mutex_unlock(&philo->left->fork);
			pthread_mutex_lock(&philo->main->synchro);
			printf("UNLOCK: %d FROM: %d\n", philo->right->fork_id, philo->id);
			pthread_mutex_unlock(&philo->main->synchro);
			pthread_mutex_unlock(&philo->right->fork);
			pthread_mutex_lock(&philo->main->synchro);
			printf("UNLOCK: %d FROM: %d\n", philo->left->fork_id, philo->id);
			pthread_mutex_unlock(&philo->main->synchro);
		}
		// pthread_mutex_unlock(&philo->right->fork);
		// pthread_mutex_lock(&philo->main->synchro);
		// printf("UNLOCK: %d FROM: %d\n", philo->left->fork_id, philo->id);
		// pthread_mutex_unlock(&philo->main->synchro);
	}
	// pthread_mutex_lock(&philo->main->synchro);
	// printf("FINISHE EATING: %d\n", philo->id);
	// pthread_mutex_unlock(&philo->main->synchro);
}

int	read_died(t_philo *philo)
{
	int	died;

	pthread_mutex_lock(&(philo->main->died_mtx));
	died = philo->main->died;
	pthread_mutex_unlock(&(philo->main->died_mtx));
	return (died);
}

int	time_up(t_philo *philo)
{
	t_time	cur;
	gettimeofday(&cur, NULL);
	pthread_mutex_lock(&(philo->main->synchro));
	if (((size_t)cur.tv_sec * 1000 + (size_t)cur.tv_usec / 1000)
					- ((size_t)(philo->main->cur).tv_sec * 1000 + (size_t)(philo->main->cur).tv_usec / 1000)
					> (((size_t)(philo->last_meal))
					- ((size_t)(philo->main->cur).tv_sec * 1000 + (size_t)(philo->main->cur).tv_usec / 1000)
					+ philo->main->die + philo->main->tresh))
		return (
			printf("THIS: %lu > %lu\n", ((size_t)cur.tv_sec * 1000 + (size_t)cur.tv_usec / 1000)
					- ((size_t)(philo->main->cur).tv_sec * 1000 + (size_t)(philo->main->cur).tv_usec / 1000)
					, (((size_t)(philo->last_meal))
					- ((size_t)(philo->main->cur).tv_sec * 1000 + (size_t)(philo->main->cur).tv_usec / 1000)
					+ philo->main->die + philo->main->tresh)),
			pthread_mutex_unlock(&(philo->main->synchro)), 1);
	pthread_mutex_unlock(&(philo->main->synchro));
	return (0);
}

int	sleeping(t_philo *philo)
{
	t_time	cur;
	t_time	timestamp;

	gettimeofday(&cur, NULL);
	pthread_mutex_lock(&(philo->main->synchro));
	printf("%ld: %d is sleeping\n", ((size_t)cur.tv_sec
		* 1000 + (size_t)cur.tv_usec / 1000)
			- ((size_t)(philo->main->cur).tv_sec
			* 1000 + (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
	pthread_mutex_unlock(&(philo->main->synchro));
	gettimeofday(&timestamp, NULL);
	while ((size_t)cur.tv_sec * 1000 + (size_t)cur.tv_usec / 1000
		< (size_t)timestamp.tv_sec * 1000
			+ (size_t)timestamp.tv_usec / 1000 + philo->main->sleep)
	{
		if (read_died(philo))
			return (0);
		gettimeofday(&cur, NULL);
	}
	return (1);
}

int	thinking(t_philo *philo)
{
	t_time	cur;

	gettimeofday(&cur, NULL);
	if (!read_died(philo))
	{
		pthread_mutex_lock(&(philo->main->synchro));
		printf("%ld: %d is thinking\n", ((size_t)cur.tv_sec * 1000
			+ (size_t)cur.tv_usec / 1000) - ((size_t)(philo->main->cur).tv_sec
				* 1000 + (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
		pthread_mutex_unlock(&(philo->main->synchro));
	}		
	return (1);
}

void	ft_usleep(long long time_in_ms)
{
	long long	start_time;

	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < time_in_ms)
		usleep(10);
}

void	initial_delay(t_philo *philo)
{
	if (philo->id % 2 == 0)
		ft_usleep(100);
	else
		ft_usleep(200);
}

void plus_counter(t_philo *philo)
{
	pthread_mutex_lock(&(philo->main->counter_mtx));
	philo->main->counter--;
	pthread_mutex_unlock(&(philo->main->counter_mtx));
}

int read_counter(t_philo *philo)
{
	int val;

	pthread_mutex_lock(&(philo->main->counter_mtx));
	val = philo->main->counter;
	pthread_mutex_unlock(&(philo->main->counter_mtx));
	if (val <= 0)
		return (0);
	else
		return (1);
}

void *start(void *arg)
{
	t_philo *philo;

	philo = (t_philo *)arg;
	pthread_mutex_lock(&(philo->main->start));
	pthread_mutex_unlock(&(philo->main->start));
	initial_delay(philo);
	while (!read_died(philo) && read_meals(philo))
	{
		// if ((philo->id % 2 == 0 && !read_even(philo))
		// 	|| ((philo->id % 2 != 0
		// 		// || (philo->main->num_philos % 2 != 0
		// 		// 	&& philo->id == philo->main->num_philos))
		// 		&& read_even(philo))))
		// {
		eat(philo);
			// plus_counter(philo);
			// if (philo->id % 2 == 0)
			// else
				// plus_counter(philo);
			// if (!read_counter(philo))
			// 	set_even(philo, 1);
			// printf("EVEN: %d\n", read_even(philo));
		if (!read_died(philo) && read_meals(philo)
				&& sleeping(philo))
			thinking(philo);
		// }
	}
	return NULL;
}

int init_part_two(main_info **main)
{
	int i;

	i = -1;
	gettimeofday(&((*main)->cur), NULL);
	pthread_mutex_lock(&(*main)->start);
	while (++i < (*main)->num_philos)
	{
		// printf("NA LOS EX\n");
		// (*main)->philos[i].eaten = 0;
		(*main)->philos[i].died = 0;
		set_start(&(*main)->philos[i]);
		if (pthread_create(&(*main)->philos[i].thread_id, NULL,
			start, &(*main)->philos[i]) != 0)
			return (pthread_mutex_unlock(&(*main)->start), printf("Error: pthread_create\n"), 1);
	}
	pthread_mutex_unlock(&(*main)->start);
	// usleep(1000);
	// printf("writing.. \n");
	// pthread_mutex_lock(&(*main)->synchro);
	// (*main)->ready = 1;
	// if (pthread_mutex_unlock(&(*main)->synchro) != 0)
	//     printf("UNLOCK PROBLEM\n");
	// printf("writing done. \n");
	return (0);
}


int init_part_one(main_info **main)
{
	int i;

	i = -1;
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
		if (i % 10 == 0)
			(*main)->tresh++;
		(*main)->philos[i].id = i + 1;
		(*main)->philos[i].left = &(*main)->forks[i];
		// printf("%d: FORK LEFT: %d\n", i, (*main)->philos[i].left->fork_id);
		(*main)->philos[i].right = &(*main)->forks[(i + 1)
			% ((*main)->num_philos)];
		// printf("%d: FORK RIGHT: %d\n", i, (*main)->philos[i].right->fork_id);
		(*main)->philos[i].meals = (*main)->meals;
		(*main)->philos[i].main = *main;
	}
	calc_tresh(*main);
	return (0);
}

int init(main_info *main, int *argv)
{
	main->philos = malloc(sizeof(t_philo) * (argv[0]));
	if (!main->philos)
		return (1);
	main->forks = malloc(sizeof(t_fork) * (argv[0]));
	if (!main->forks)
		return (1);
	main->num_philos = argv[0];
	if (main->num_philos % 2 == 0)
		main->counter = main->num_philos / 2;
	else
		main->counter = (main->num_philos - 1) / 2;
	main->die = argv[1];
	main->eat = argv[2];
	main->sleep = argv[3];
	main->meals = argv[4];
	main->even = 0;
	main->died = 0;
	if (pthread_mutex_init(&main->synchro, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->counter_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->meals_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->died_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->even_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->start, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (init_part_one(&main) != 0)
		return (1);
	if (init_part_two(&main) != 0)
		return (1);
	return (0);
}

int	check_died(main_info *main)
{
	int i;
	int count;
	int cur_meals;
	t_time	cur;

	i = -1;
	count = main->num_philos;
	while (++i < main->num_philos && count)
	{
		cur_meals = read_meals(&main->philos[i]);
		if (!cur_meals)
			count--;
		if (!count)
			return (1);
		if (time_up(&main->philos[i]) && cur_meals)
		{
			gettimeofday(&cur, NULL);
			printf("%ld: %d died\n", ((size_t)cur.tv_sec * 1000
			+ (size_t)cur.tv_usec / 1000) - ((size_t)(main->cur).tv_sec
			* 1000 + (size_t)(main->cur).tv_usec / 1000), main->philos[i].id);
			return (1);
		}
	}
	return (0);
}

int run_the_rest(main_info *main)
{
	int i;

	pthread_mutex_lock(&main->start);
	pthread_mutex_unlock(&main->start);
	while (!(check_died(main)))
		usleep(1000);
	pthread_mutex_lock(&main->died_mtx);
	main->died = 1;
	pthread_mutex_unlock(&main->died_mtx);
	i = -1;
	while (++i < main->num_philos)
	{
		if (pthread_join(main->philos[i].thread_id, NULL) != 0)
			return (printf("Error: pthread_join\n"), 1);
	}
	i = -1;
	while (++i < main->num_philos)
	{
		// printf("Destroy %d\n", main->philos[i].left->fork_id);
		if (pthread_mutex_destroy(&main->philos[i].left->fork) != 0)
			return (1);
		if (pthread_mutex_destroy(&main->philos[i].phil_mtx) != 0)
			return (1);
	}
	if (pthread_mutex_destroy(&main->synchro) != 0)
		// return (printf("Error: pthread_mutex_destroy\n"), 1);
		return (1);
	if (pthread_mutex_destroy(&main->died_mtx) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->meals_mtx) != 0)
		return (1);
		// return (printf("Error: pthread_mutex_destroy\n"), 1);
	if (pthread_mutex_destroy(&main->even_mtx) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->counter_mtx) != 0)
		return (1);
	if (pthread_mutex_destroy(&main->start) != 0)
		return (1);
		// return (printf("Error: pthread_mutex_destroy\n"), 1);
	free(main->philos);
	free(main->forks);
	return (0);
}

int main(int argc, char **argv)
{
	main_info main;
	int arr[6];
	int i;

	i = -1;
	arr[5] = 0;
	if (argc == 5 || argc == 6)
	{
		if (!check_in(argv + 1))
			return (printf("Invalid input!\n"), 0);
		// arr[0] = (int)ft_atoi(argv[1]);
		while (argv[++i + 1])
			arr[i] = (int)ft_atoi(argv[i + 1]);
		if (argc == 5)
			arr[i] = -1;
		if (init(&main, arr) != 0)
			return (1);
		if (run_the_rest(&main) != 0)
			return (1);
	} 
	else
		return (printf("Invalid argc!\n"), 0);
	return (0);
}

