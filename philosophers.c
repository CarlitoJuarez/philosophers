#include "philosophers.h"

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
	philo->last_meal = ((size_t)cur.tv_sec * 1000
		+ (size_t)cur.tv_usec / 1000);
	// printf("SET_TIME %d: %ld\n", philo->id, philo->last_meal - ((size_t)(philo->main->cur).tv_sec * 1000
	// 			+ (size_t)(philo->main->cur).tv_usec / 1000));
	return (1);
}

void set_start(t_philo *philo)
{
	philo->last_meal = ((size_t)philo->main->cur.tv_sec * 1000
		+ (size_t)philo->main->cur.tv_usec / 1000);
	// printf("SET_TIME %d: %ld\n", philo->id, philo->last_meal - ((size_t)(philo->main->cur).tv_sec * 1000
	// 			+ (size_t)(philo->main->cur).tv_usec / 1000));
}

void	print_timestamp_fork(t_philo *philo)
{
	t_time	cur;

	gettimeofday(&cur, NULL);
	// pthread_mutex_lock(&philo->main->synchro);
	printf("%ld: %d has taken a fork\n", ((size_t)cur.tv_sec
		* 1000 + (size_t)cur.tv_usec / 1000)
			- ((size_t)(philo->main->cur).tv_sec * 1000
				+ (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
	// pthread_mutex_unlock(&philo->main->synchro);
}

long long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000LL) + (tv.tv_usec / 1000));
}

int possible(t_philo *philo)
{
	// int tresh;
	// int this;

	// tresh = philo->main->die - (philo->main->eat * 2 + philo->main->sleep);
	// this = tresh / 2;
	if (philo->main->num_philos % 2 == 0 && philo->main->die > philo->main->eat + philo->main->sleep)
		return (usleep(40000 * philo->main->tresh), 1);
	if (philo->main->die == philo->main->eat * 2 + philo->main->sleep)
		// return (usleep(10000), 1);
		return (0);
	if (philo->main->die > philo->main->eat * 2 + philo->main->sleep - philo->main->tresh)
		return (usleep(8000 * philo->main->tresh), 1);

	// if (philo->main->die > philo->main->eat * 2 + philo->main->sleep)
	// 	return (0);
	return (0);
}

void calc_tresh(main_info *main)
{
	int tresh_2;

	// if (main->num_philos % 2 == 0)
	// 	return ;
	tresh_2 = main->die - (main->eat * 2 + main->sleep);
	if (tresh_2 < 0)
		tresh_2 = 1;
	if (tresh_2 > 20)
		tresh_2 = 5;

	if (main->num_philos < 10)
		main->tresh = 1;
	// if (main->tresh < tresh_2)
		// main->tresh = tresh_2;
	if (main->tresh > tresh_2)
			main->tresh = tresh_2;
}

void	custom_usleep(long long time_in_ms,t_philo *philo)
{
	long long	start_time;

	start_time = get_time_in_ms();
	// if ((philo->main->num_philos % 2 == 0 && printf("EVEN\n") && philo->main->num_philos > 100) || (philo->main->num_philos > 100 && philo->main->die > 200))
	// if ((philo->main->num_philos % 2 == 0 && printf("EVEN\n") && philo->main->num_philos > 100) || (philo->main->die % 2 != 0 && philo->main->num_philos > 100 && philo->main->die > 200))
	// if ((philo->main->num_philos % 2 == 0 && printf("EVEN\n") && philo->main->num_philos > 100))
	// if (philo->main->die % 2 != 0 || (philo->main->num_philos < 100 && philo->main->die < 200))
	// if (philo->main->num_philos % 2 == 0 || (possible(philo)))
	if ((possible(philo)))
		set_time(philo);
	while (!read_died(philo)
		&& (get_time_in_ms() - start_time) < time_in_ms)
		usleep(100);
}

int minus_meals(t_philo *philo)
{
	if (philo->id == 0 || philo->id == 1)
		philo->meals--;
	return (1);
}

int lock_forks(t_philo *philo)
{
	if (philo->left->fork_id == philo->right->fork_id)
	{
		pthread_mutex_lock(&philo->left->fork);
		print_timestamp_fork(philo);
		return (pthread_mutex_unlock(&philo->left->fork), 0);
	}
	set_time(philo);
	if (philo->id % 2 == 0 && !read_died(philo))
	{
		pthread_mutex_lock(&philo->left->fork);
		pthread_mutex_lock(&philo->right->fork);
	}
	else if (!read_died(philo))
	{
		pthread_mutex_lock(&philo->right->fork);
		pthread_mutex_lock(&philo->left->fork);
	}
	if (!read_died(philo) && !time_up(philo))
	{
		print_timestamp_fork(philo);
		print_timestamp_fork(philo);
	}
	return (1);
}

void	eat(t_philo *philo)
{
	t_time	cur;

	if (philo->meals-- && !read_died(philo) && !time_up(philo) && lock_forks(philo))
	// if (((read_meals(philo) >= 0 && read_even(philo))
	// 	|| read_meals(philo)) && minus_meals(philo))
	{
		// printf("%d: ACESSING FORK: %d\n", philo->id, philo->left->fork_id);
		// custom_usleep(1, philo);
		// if (philo->id % 2 == 0)
		// printf("%d: ACESSING FORK: %d\n", philo->id, philo->right->fork_id);
		// print_timestamp_fork(philo);
		// (philo->eaten) = 1;
		// pthread_mutex_lock(&philo->main->synchro);
		// set_time(philo);
		gettimeofday(&cur, NULL);
		if (!read_died(philo))
			printf("%ld: %d is eating\n", ((size_t)cur.tv_sec * 1000
				+ (size_t)cur.tv_usec / 1000) - ((size_t)(philo->main->cur).tv_sec
				* 1000 + (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
		// pthread_mutex_unlock(&philo->main->synchro);
		// if (philo->main->num_philos % 2 == 0)
		// 	set_time(philo);
		custom_usleep(philo->main->eat, philo);
		// if ((possible(philo)))
		// 	set_time(philo);
		// usleep(philo->main->eat * 1000);
		// set_time(philo);

		// printf("EAT FOR: %d\n", philo->main->eat * 1000);
		// printf("PHILO: %d | LAST MEAL: %ld\n", philo->id, phi	lo->last_meal);
		pthread_mutex_unlock(&philo->right->fork);
		pthread_mutex_unlock(&philo->left->fork);
	}
}

int	read_died(t_philo *philo)
{
	int	died;

	pthread_mutex_lock(&(philo->main->died_mtx));
	died = philo->main->died;
	pthread_mutex_unlock(&(philo->main->died_mtx));
	return (died);
}

void	die(t_philo *philo)
{
	t_time	cur;

	gettimeofday(&cur, NULL);
	if (!read_died(philo))
	{
		pthread_mutex_lock(&philo->main->died_mtx);
		printf("%ld: %d died\n", ((size_t)cur.tv_sec * 1000
		+ (size_t)cur.tv_usec / 1000) - ((size_t)(philo->main->cur).tv_sec
			* 1000 + (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
		philo->main->died = 1;
		pthread_mutex_unlock(&(philo->main->died_mtx));
	}
}

int	time_up(t_philo *philo)
{
	t_time	cur;

	gettimeofday(&cur, NULL);
	if (((size_t)cur.tv_sec * 1000 + (size_t)cur.tv_usec / 1000)
					- ((size_t)(philo->main->cur).tv_sec * 1000 + (size_t)(philo->main->cur).tv_usec / 1000)
					> (((size_t)(philo->last_meal))
					- ((size_t)(philo->main->cur).tv_sec * 1000 + (size_t)(philo->main->cur).tv_usec / 1000)
					+ philo->main->die))
		return (1);
	return (0);
}

int	sleeping(t_philo *philo)
{
	t_time	cur;
	t_time	timestamp;

	gettimeofday(&cur, NULL);
	printf("%ld: %d is sleeping\n", ((size_t)cur.tv_sec
		* 1000 + (size_t)cur.tv_usec / 1000)
			- ((size_t)(philo->main->cur).tv_sec
			* 1000 + (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
	gettimeofday(&timestamp, NULL);
	while ((size_t)cur.tv_sec * 1000 + (size_t)cur.tv_usec / 1000
		< (size_t)timestamp.tv_sec * 1000
			+ (size_t)timestamp.tv_usec / 1000 + philo->main->sleep)
	{
		if (time_up(philo))
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
		printf("%ld: %d is thinking\n", ((size_t)cur.tv_sec * 1000
			+ (size_t)cur.tv_usec / 1000) - ((size_t)(philo->main->cur).tv_sec
				* 1000 + (size_t)(philo->main->cur).tv_usec / 1000), philo->id);
	}
	return (1);
}


int	read_even(t_philo *philo)
{
	int	even;

	even = philo->main->even;
	return (even);
}

void	set_even(t_philo *philo, int val)
{
	pthread_mutex_lock(&(philo->main->even_mtx));
	philo->main->even = val;
	pthread_mutex_unlock(&(philo->main->even_mtx));
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

int	read_meals(t_philo *philo)
{
	int i;

	i = philo->meals;
	return (i);
}

void	sync_simulation(t_philo *philo)
{
	pthread_mutex_lock(&(philo->main->synchro));
	pthread_mutex_unlock(&(philo->main->synchro));
	initial_delay(philo);
}

int	handle_meals(t_philo *philo)
{
	if (!philo->meals)
	{
		pthread_mutex_lock(&(philo->main->died_mtx));
		philo->main->died = 1;
		pthread_mutex_unlock(&(philo->main->died_mtx));
		return (0);
	}
	return (1);
}

void	handle_eat(t_philo *philo, int even)
{
	if (even)
	{
		set_even(philo, 1);
		eat(philo);
		set_even(philo, 0);
	}
}


void *start(void *arg)
{
	t_philo *philo;

	philo = (t_philo *)arg;
	sync_simulation(philo);
	while (!read_died(philo) && philo->meals)
	// while (!read_died(philo) && handle_meals(philo))
	{
		if (philo->meals && !read_died(philo) && time_up(philo))
			return (die(philo), NULL);
		if (philo->meals && philo->id % 2 == 0 && read_even(philo))
			handle_eat(philo, 1);
		if (philo->meals && philo->id % 2 != 0 && !read_even(philo))
		{
			set_even(philo, 0);
			eat(philo);
			set_even(philo, 1);
		}
		if (philo->eaten && philo->meals && !read_died(philo)
				&& (philo->eaten) && sleeping(philo))
			thinking(philo);
	}
	pthread_mutex_lock(&(philo->main->died_mtx));
	philo->main->died = 1;
	pthread_mutex_unlock(&(philo->main->died_mtx));
	return NULL;
}

int init_part_two(main_info **main)
{
	int i;

	i = -1;
	gettimeofday(&((*main)->cur), NULL);
	pthread_mutex_lock(&(*main)->synchro);
	while (++i < (*main)->num_philos)
	{
		// printf("NA LOS EX\n");
		// (*main)->philos[i].eaten = 0;
		(*main)->philos[i].died = 0;
		set_start(&(*main)->philos[i]);
		if (pthread_create(&(*main)->philos[i].thread_id, NULL,
			start, &(*main)->philos[i]) != 0)
			return (pthread_mutex_unlock(&(*main)->synchro), printf("Error: pthread_create\n"), 1);
	}
	pthread_mutex_unlock(&(*main)->synchro);
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
		(*main)->forks[i].fork_id = i + 1;
		if (pthread_mutex_init(&(*main)->forks[i].fork, NULL) != 0)
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
	printf("TRESH: %d\n", (*main)->tresh);
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
	main->die = argv[1];
	main->eat = argv[2];
	main->sleep = argv[3];
	main->meals = argv[4];
	// printf("MEALS START: %d\n", main->meals);
	main->even = 1;
	main->died = 0;
	if (pthread_mutex_init(&main->synchro, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->meals_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->died_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (pthread_mutex_init(&main->even_mtx, NULL) != 0)
		return (printf("Error: pthread_mutex_init\n"), 1);
	if (init_part_one(&main) != 0)
		return (1);
	if (init_part_two(&main) != 0)
		return (1);
	return (0);
}

int run_the_rest(main_info *main)
{
	int i;

	while (!(read_died(&main->philos[0])))
		usleep(100);
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
	}
	if (pthread_mutex_destroy(&main->synchro) != 0)
		// return (printf("Error: pthread_mutex_destroy\n"), 1);
		return (1);
	if (pthread_mutex_destroy(&main->died_mtx) != 0)
		return (1);
		// return (printf("Error: pthread_mutex_destroy\n"), 1);
	if (pthread_mutex_destroy(&main->even_mtx) != 0)
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

