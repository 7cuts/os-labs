# Сравнение

вот [табличка](https://docs.google.com/spreadsheets/d/1FxIc-AOMl17FTb6Q9oKYMGsUiWHnZcNyE7pAfu0D58Q/edit?usp=sharing).
если кратко, особо разницы не заметил ....

# Мне стало интересно, почему у всех примерно одинаковое время sys..

Написал все синхронизации

## Насчёт семафора.

Я видел 2 подхода, из которых я выбрал второй.
1. Делать синхронизацию через бинарный семафор. sem_wait как lock(), а sem_post как unlock().
2. Делать синхронизацию через мьютексы, а для семафора сделать переменные `count` и `free`,
   которые как раз будут блокироваться на sem_wait, когда очередь либо полна, либо пуста.


Увидел, что реализация на mutex и на spinlock не отличаются вообще.
time показывает одинаковое sys, user время и для тех, и для тех.

Делаю `strace -c -f ./queue-threads-mutex 100000000`

```
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ------------------
 81,46    7,801507      709227        11         1 clock_nanosleep    -- это понятно, скип
 18,52    1,773832           3    463973    160269 futex              -- на pthread_mutex_lock
  0,00    0,000473         473         1           execve
  0,00    0,000348          13        25           write
  0,00    0,000244          14        17           mmap
```

```sh
strace -c -f   ./queue-threads-spinlock 100000000
```

```
:: queue freed
% time     seconds  usecs/call     calls    errors syscall
------ ----------- ----------- --------- --------- ------------------
 62,27    4,533897     4533897         1           clock_nanosleep
 37,67    2,742890           5    481792    203802 futex          -- значит не только на pthread_mutex_lock.
  0,02    0,001594           4       357           mprotect
```

вот [тут](https://elixir.bootlin.com/glibc/glibc-2.44.9000/source/malloc/malloc.c)
на 1560 строчке:

```c
struct malloc_state
{
  /* Serialize access.  */
  __libc_lock_define (, mutex);
/* ... */
```

malloc использует mutex, а мы вызываем его при каждом добавлении элемента.
тут можно только pool allocator писать, чтобы оптимизировать это.

# Добавляю usleep(1) во writer

Важно отметить, что в mutex с usleep(10) очень сильно падает загрузка cpu по
сравнению с чистым mutex или sched_yield(), а количество get/add не сильно поменялось.

usleep я добавил в места, где:

- reader понимает, что очередь пуста
- writer понимает, что очередь заполнена.

Поток засыпает, давай другому кванты времени на поработать.

Замеры (каждый раз запускаю на ~10 секунд).

```sh
./queue-threads-spinlock 100000000  10,41s user 0,08s system 104% cpu 10,004 total
get_count: 178694
add_count: 178694

./queue-threads-condvar 100000000  0,81s user 0,99s system 18% cpu 10,004 total
get_count: 169811
add_count: 169811

# версия со sched_yied() (так же, как и без него)
./queue-threads-mutex 100000000  2,38s user 8,13s system 105% cpu 10,004 total
get_count: 178363
add_count: 178363

# версия с usleep(10)
./queue-threads-mutex 100000000  0,53s user 0,58s system 11% cpu 10,004 total
get_count: 172838
add_count: 172838

# версия с usleep(1). get add около 170к
./queue-threads-mutex 100000000  0,43s user 0,77s system 11% cpu 10,004 total

# версия с usleep(1) во writer и с usleep(10) в reader
# моя логика была в том, что если writer ещё и делает usleep(1) в цикле
# (типа ждёт что-то из сети), то ридеру лучше подольше поспать, освободив кванты времени
# для writera... но а в чём смысл освобождать время для него, если он сидит на типа epoll????
./queue-threads-mutex 100000000  0,41s user 0,69s system 11% cpu 10,004 tota
get_count: 173403
add_count: 173404

./queue-threads-semaphore 100000000  0,69s user 0,95s system 16% cpu 10,004 total
get_count: 169792
add_count: 169792
```

я попробовал с n == 100, 100000000 -- то же самое.
