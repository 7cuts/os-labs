Почему при выставлении set_cpu на разные ядра, 
writer забивает очередь и reader не успевает

## Меняем размер очереди

При параметре `q = queue_init(200000);` у меня почти сразу выпадает 
segfault с таким выводом:

```c
reader: q->first == NULL, q->count = 98685
```

Если брать меньше, чем 200_000, то add_attempts-get_attemts отрицательное.
Если больше, то положительное.


# Наблюдаемые ошибки
1. ERROR: get value is 9130025 but expected - 9130024
2. segfault в reader: queue_get(). q->first == NULL но q->count > 0
