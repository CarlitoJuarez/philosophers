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


void eat(t_philo *philo)
{
    t_time cur;

    // gettimeofday(&cur, NULL);
    // printf("%d: %d MEALS: %d\n", (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000, philo->id, philo->meals);
    if (philo->meals)
    {
        pthread_mutex_lock(&philo->left->fork);
        gettimeofday(&cur, NULL);
        printf("%d: %d has taken a fork: %d\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id, philo->left->fork_id);
        pthread_mutex_lock(&philo->right->fork);
        gettimeofday(&cur, NULL);
        printf("%d: %d has taken a fork: %d\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id, philo->right->fork_id);
        (philo->eaten) = 1;
        printf("%d: %d is eating\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
        usleep(philo->main->eat * 1000);
        pthread_mutex_unlock(&philo->left->fork);
        gettimeofday(&cur, NULL);
        // printf("%d: %d has released a fork: %d\n", (int)current_time.tv_usec / 1000, philo->id, philo->left->fork_id);
        pthread_mutex_unlock(&philo->right->fork);
        gettimeofday(&cur, NULL);
        // printf("%d: %d has released a fork: %d\n", (int)current_time.tv_usec / 1000, philo->id, philo->right->fork_id);
        (philo->last_meal) = (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000;
        philo->meals--;
    }
}

int time_up(t_philo *philo)
{
    t_time cur;

    gettimeofday(&cur, NULL);
    // if (philo->id == 0)
        // printf("%d: %d DIE: %d + LAST: %d < CUR: %d \n", (int)current_time.tv_usec / 1000, philo->id, philo->main->die, *(philo->last_meal), (int)current_time.tv_usec / 1000);
    // printf("last: %d\n", *(philo->last_meal));
    // printf("time: %d\n", (int)current_time.tv_usec / 1000);
    if ((philo->main->die) + (philo->last_meal) <= (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000)
        return (1);
    return (0);
}

int sleeping(t_philo *philo)
{
    t_time cur;
    t_time timestamp;

    gettimeofday(&cur, NULL);
    printf("%d: %d sleeping..\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
    gettimeofday(&timestamp, NULL);
    // printf("SLEEP: %d\n", philo->main->sleep);
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
    printf("%d: %d thinking..\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
    return (1);
}

void die(t_philo *philo)
{
    t_time cur;

    gettimeofday(&cur, NULL);
    printf("%d: %d died..\n", ((int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000) % 10000, philo->id);
}

void *start(void *arg)
{
    t_philo *philo;

    philo = (t_philo *)arg;

    while ((philo->main->died) == 0)
    {
        pthread_mutex_lock(&(philo->main->synchro));
        if ((time_up(philo) && !(philo->main->died)))
            return (die(philo), (philo->main->died) = 1, NULL);
        pthread_mutex_unlock(&(philo->main->synchro));
        if (philo->id % 2 == 0)
        {
            (philo->main->even) = 1;
            eat(philo);
            (philo->main->even) = 0;
        }
        if (philo->id % 2 != 0 && !(philo->main->even))
            eat(philo);
        if (!time_up(philo) && !(philo->main->died) 
                && (philo->eaten) && sleeping(philo) && thinking(philo))
            (philo->eaten) = 0;
    }
    return NULL;
}

int init_part_two(main_info **main)
{
    t_time cur;
    int i;
    // int *eaten;
    // int *died;
    // int *last_meal;

    i = -1;
    gettimeofday(&cur, NULL);
    while (++i < (*main)->num_philos)
    {
        // eaten = malloc(sizeof(int));
        // *eaten = 0;
        // died = malloc(sizeof(int));
        // *died = 0;
        // last_meal = malloc(sizeof(int));
        // *last_meal = (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000;
        (*main)->philos[i].eaten = 0;
        (*main)->philos[i].died = 0;
        (*main)->philos[i].last_meal = (int)cur.tv_sec * 1000 + (int)cur.tv_usec / 1000;
        if (pthread_create(&(*main)->philos[i].thread_id, NULL, &start, &(*main)->philos[i]) != 0)
            return (printf("Error: pthread_create\n"), 1);
        usleep(10);
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
        // printf("PHILO: %d has left: %d and right: %d\n", (*main)->philos[i].id, ((*main)->philos[i].left->fork_id), ((*main)->philos[i].right->fork_id));
        (*main)->philos[i].main = *main;
    }
    return (0);
}

int init(main_info *main, int *argv)
{
    // int *even;
    // int *died;

    // even = malloc(sizeof(int));
    // *even = 1;
    // died = malloc(sizeof(int));
    // *died = 0;
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
    // TODO SYNCHRO
    if (pthread_mutex_init(&main->synchro, NULL) != 0)
        return (printf("Error: pthread_mutex_init\n"), 1);
    // END_TODO
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
    // pthread_t kill_thread;
    // if (!pthread_create(&kill_thread, NULL, kill_function, &main))
    //     return (0);

    while (!(main->died))
        ;
    i = -1;
    while (++i < main->num_philos)
        pthread_detach(main->philos[i].thread_id);
    i = -1;
    while (++i < main->num_philos)
        pthread_mutex_destroy(&main->philos[i].left->fork);
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

