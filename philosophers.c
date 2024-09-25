#include "philosophers.h"

int check_in(char **arr)
{
    long i;

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

void print_timestamp_fork(t_philo *philo, int fork_id)
{
    t_time cur;

    gettimeofday(&cur, NULL);
    pthread_mutex_lock(&philo->main->synchro);
    printf("%d: %d has taken a fork: %d\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id, fork_id);
    pthread_mutex_unlock(&philo->main->synchro);
}

void eat(t_philo *philo)
{
    t_time cur;

    if (philo->meals)
    {
        pthread_mutex_lock(&philo->left->fork);
        print_timestamp_fork(philo, philo->left->fork_id);
        pthread_mutex_lock(&philo->right->fork);
        print_timestamp_fork(philo, philo->right->fork_id);
        (philo->eaten) = 1;
        pthread_mutex_lock(&philo->main->synchro);
        gettimeofday(&cur, NULL);
        printf("%d: %d is eating\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
        pthread_mutex_unlock(&philo->main->synchro);
        usleep(philo->main->eat * 1000);
        pthread_mutex_unlock(&philo->left->fork);
        pthread_mutex_unlock(&philo->right->fork);
        gettimeofday(&cur, NULL);
        (philo->last_meal) = (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000;
        philo->meals--;
    }
}

int time_up(t_philo *philo)
{
    t_time cur;

    gettimeofday(&cur, NULL);
    if ((philo->main->die) + (philo->last_meal) <= (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000)
        return (1);
    return (0);
}

int sleeping(t_philo *philo)
{
    t_time cur;
    t_time timestamp;

    gettimeofday(&cur, NULL);
    pthread_mutex_lock(&philo->main->synchro);
    printf("%d: %d sleeping..\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
    pthread_mutex_unlock(&philo->main->synchro);
    gettimeofday(&timestamp, NULL);
    while ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000 < (int)timestamp.tv_sec * 1000 + (int)timestamp.tv_usec / 1000 + philo->main->sleep)
    {
        if (time_up(philo))
            return (0);
        usleep(1000);
        gettimeofday(&cur, NULL);
    }
    return (1);
}

int thinking(t_philo *philo)
{
    t_time cur;

    gettimeofday(&cur, NULL);
    pthread_mutex_lock(&philo->main->synchro);
    printf("%d: %d thinking..\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
    pthread_mutex_unlock(&philo->main->synchro);
    return (1);
}

void die(t_philo *philo)
{
    t_time cur;

    gettimeofday(&cur, NULL);
    pthread_mutex_lock(&philo->main->synchro);
    printf("%d: %d died..\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
    philo->main->died = 1;
    pthread_mutex_unlock(&(philo->main->synchro));
}

int read_died(t_philo *philo)
{
    int died;

    pthread_mutex_lock(&(philo->main->synchro));
    died = philo->main->died;
    pthread_mutex_unlock(&(philo->main->synchro));
    return (died);
}

int read_even(t_philo *philo)
{
    int even;

    pthread_mutex_lock(&(philo->main->synchro));
    even = philo->main->even;
    pthread_mutex_unlock(&(philo->main->synchro));
    return (even);
}

void    set_even(t_philo *philo, int val)
{
    pthread_mutex_lock(&(philo->main->synchro));
    philo->main->even = val;
    pthread_mutex_unlock(&(philo->main->synchro));
}

void *start(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;

    while (!read_died(philo))
    {
        pthread_mutex_lock(&(philo->main->synchro1));
        if ((time_up(philo) && !read_died(philo)))
            return (die(philo), pthread_mutex_unlock(&(philo->main->synchro1)), NULL);
        pthread_mutex_unlock(&(philo->main->synchro1));
        if (philo->id % 2 == 0)
        {
            set_even(philo, 1);
            eat(philo);
            set_even(philo, 0);
        }
        if (philo->id % 2 != 0 && !read_even(philo))
            eat(philo);
        if (!time_up(philo) && !read_died(philo)
                && (philo->eaten) && sleeping(philo) && thinking(philo))
            (philo->eaten) = 0;
    }
    return NULL;
}

int init_part_two(main_info **main)
{
    t_time cur;
    int i;

    i = -1;
    gettimeofday(&cur, NULL);
    while (++i < (*main)->num_philos)
    {
        (*main)->philos[i].eaten = 0;
        (*main)->philos[i].died = 0;
        (*main)->philos[i].last_meal = (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000;
        if (pthread_create(&(*main)->philos[i].thread_id, NULL, &start, &(*main)->philos[i]) != 0)
            return (printf("Error: pthread_create\n"), 1);
    }
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
    }
    i = -1;
    while (++i < (*main)->num_philos)
    {
        (*main)->philos[i].id = i;
        (*main)->philos[i].id = i;
        (*main)->philos[i].id = i;
        (*main)->philos[i].left = &(*main)->forks[i];
        (*main)->philos[i].right = &(*main)->forks[(i + 1) % ((*main)->num_philos)];
        (*main)->philos[i].meals = (*main)->meals;
        (*main)->philos[i].main = *main;
    }
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
    main->even = 1;
    main->died = 0;
    if (pthread_mutex_init(&main->synchro, NULL) != 0)
        return (printf("Error: pthread_mutex_init\n"), 1);
    if (pthread_mutex_init(&main->synchro1, NULL) != 0)
        return (printf("Error: pthread_mutex_init\n"), 1);
    if (init_part_one(&main) != 0)
        return (1);
    if (init_part_two(&main) != 0)
        return (1);
    return (0);
}

void *kill_function(void *arg)
{
    main_info *main;

    main = (main_info *)arg;
    while (!main->died)
        ;
    return NULL;
}

int run_the_rest(main_info *main)
{
    int i;

    while (!(main->died))
        ;
    i = -1;
    while (++i < main->num_philos)
    {
        if (pthread_detach(main->philos[i].thread_id) != 0)
            return (printf("Error: pthread_detach\n"), 1);
    }
    i = -1;
    while (++i < main->num_philos)
    {
        if (pthread_mutex_destroy(&main->philos[i].left->fork) != 0)
            return (printf("Error: pthread_mutex_destroy\n"), 1);
    }
    if (pthread_mutex_destroy(&main->synchro) != 0)
        return (printf("Error: pthread_mutex_destroy\n"), 1);
    if (pthread_mutex_destroy(&main->synchro1) != 0)
        return (printf("Error: pthread_mutex_destroy\n"), 1);
    free(main->philos);
    free(main->forks);
    return (0);
}

int main(int argc, char **argv)
{
    main_info main;
    int arr[6];
    int i;

    i = 0;
    arr[5] = 0;
    if (argc == 5 || argc == 6)
    {
        if (!check_in(argv + 1))
            return (printf("Invalid input!\n"), 0);
        arr[0] = (int)ft_atoi(argv[1]);
        while (argv[++i + 1])
            arr[i] = (int)ft_atoi(argv[i + 1]);
        if (argc == 5)
            arr[i] = -1;
        if (init(&main, arr) != 0)
            return (1);
        if (run_the_rest(&main) != 0)
            return (1);
        printf("FINISHED\n");
    } 
    else
        return (printf("Invalid argc!\n"), 0);
    return (0);
}

