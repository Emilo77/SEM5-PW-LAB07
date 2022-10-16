# Laboratorium 8 - współbieżność w systemie operacyjnym: procesy

Do laboratorium zostały dołączone pliki: 
- [CmakeLists.txt](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/CMakeLists.txt)
- [err.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/err.c)
- [err.h](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/err.h)
- [exec.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/exec.c)
- [fork.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/fork.c)
- [hello-world.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/hello-world.c)
- [tree.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/tree.c)
- [wait.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/wait.c)

## 1 Wstęp - funkcje systemowe

Każda z funkcji, którą będziemy omawiać wymaga pewnych działań systemu operacyjnego, a dokładniej - wykonania **funkcji systemowych**. Pisząc program nie używamy bezpośrednio funkcji systemowych, ale odpowiadających im funkcji bibliotecznych, które wywołują  właściwe funkcje i wykonują pewne dodatkowe czynności.

Funkcja systemowa nie jest zwykłym wywołaniem funkcji. Jest wykonanie oznacza przejście w tryb uprzywilejowany i zlecenie systemowi operacyjnemu wykonania pewnych czynności na rzecz procesu. Ze względu na przekraczanie granic ochrony jest to droższe niż zwykłe wywołanie funkcji.

Każda funkcja systemowa przekazuje swój kod zakończenia. Jest to 0, jeżeli funkcja zakończyła się pomyślnie lub liczba ujemna oznaczająca kod błędu w przeciwnym przypadku. Funkcja opakowująca z biblioteki C, która wywołuje funkcję systemową, sprawdza czy nastąpił błąd i jeżeli tak, to przypisuje wartość błędu na globalną zmienną procesu o nazwie  `errno` i przekazuje w wyniku -1. Dzięki kodowi błędu uzyskujemy więcej informacji o powodach wystąpienia danego błędu. Przykład wykorzystania zmiennej `errno` można znaleźć w funkcji `syserr()` w pliku [err.c](), która korzysta z globalnej tablicy sys_errlist zawierającej opisy wszystkich kodów błędów.

[err.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/err.c)

```c
extern int sys_nerr;

void syserr(const char *fmt, ...)  
{
  va_list fmt_args;

  fprintf(stderr, "ERROR: ");

  va_start(fmt_args, fmt);
  vfprintf(stderr, fmt, fmt_args);
  va_end (fmt_args);
  fprintf(stderr," (%d; %s)\n", errno, strerror(errno));
  exit(1);
}
```

W dalszym ciągu zajęć będziemy używać pewnego skrótu myślowego, a mianowicie będziemy nazywać funkcją systemową odpowiednią funkcję biblioteczną.

Zalecamy używanie funkcji  `syserr()` lub `perror()` do obsługi błędów funkcji systemowych. Można również napisać w tym celu własną funkcję, która będzie wykorzystywać informacje zawarte w zmiennej   `errno`.

## 2 Procesy
### 2.1 Identyfikator procesu

Każdy proces w systemie ma jednoznaczny **identyfikator** nazywany potocznie **pidem** (od angielskiego: *process identifier*). Identyfikatory aktualnie wykonujących się procesów można poznać wykonując polecenie `ps`.

Wykonaj polecenie `ps`. Zobaczysz wszystkie procesy uruchomione przez Ciebie w tej sesji. Znajdzie się wśród nich proces`ps` i `bash`, czyli interpreter poleceń, który analizuje i wykonuje Twoje polecenia.  Pierwsza kolumna to pid procesu, a ostatnia to polecenie, które ten proces wykonuje.

Z poziomu języka programowania proces może poznać swój pid wywołując funkcję systemową `getpid()`. Przekazywane w wyniku wartości typu `pid_t` reprezentują pidy procesów. Najczęściej jest to długa liczba całkowita, ale w zależności od wariantu systemu definicja ta może być inna.

[hello-world.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/hello-world.c)

```c
int main ()
{
  pid_t pid = getpid();
  printf("Hello from %d\n", pid);
}
```

### 2.2 Tworzenie nowego procesu


W Linuksie, tak jak we wszystkich systemach uniksowych, istnieje hierarchia procesów. Każdy proces poza pierwszym procesem w systemie (procesem init o pidzie równym 1) jest tworzony przez inny proces. Nowy proces nazywamy **procesem potomnym**, a proces który go stworzył **procesem macierzystym**.

Do tworzenia procesów służy funkcja systemowa `fork()`. Powrót z wywołania tej funkcji następuje dwa razy: w procesie macierzystymi w procesie potomnym.
Proces potomny wykonuje taki sam kod jak proces macierzysty - zaczyna od wykonania następnej instrukcji po `fork()`. Jednak przestrzenie adresowe tych procesów są rozłączne. Każdy ma swoją kopię zmiennych. Wartości zmiennych w procesie potomnym są początkowo takie same jak w procesie macierzystym w momencie utworzenia nowego procesu. Procesy mają te same uprawnienia, te same otwarte pliki itd.

[fork.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/fork.c)

```c
int main ()
{
  printf("Hello from %d\n", getpid());
  if (fork() == -1) syserr("Error in fork\n");  /* powstaje nowy proces */
  printf("Goodbye from %d\n", getpid());
  return 0;  
}
```

Dla potomka funkcja `fork()` przekazuje w wyniku 0, a dla procesu macierzystego - pid nowo utworzonego potomka.
  
### 2.3 Oczekiwanie na zakończenie procesu potomnego

Proces macierzysty może zaczekać na zakończenie procesu potomnego za pomocą funkcji `wait(int status)`.  Funkcja przekazuje w wyniku pid zakończonego procesu. Parametr `status` jest wskaźnikiem do zmiennej zawierającej kod zakończenia procesu. Funkcja jest blokująca, co oznacza, ze proces macierzysty, który ja wywoła zostanie wstrzymany, aż do zakończenia jednego ze swoich procesów potomnych. Jeżeli proces nie miał potomków, to funkcja zwróci błąd. Jeżeli potomek zakończy się zanim rodzic wywoła `wait`, to `wait` nie zablokuje procesu i wykona się poprawnie dając w wyniku pid zakończonego potomka.

System operacyjny przechowuje kody zakończenia procesów potomnych, aż do chwili odebrania ich przez procesy macierzyste. Proces potomny, którego kod nie został odebrany przez rodzica, staje się zombie, czyli procesem który się zakończył, ale informacje o nim są wciąż przechowywane przez system. Pobieranie kodów zakończenia, czyli wywoływanie `wait()` jest ważne,  ponieważ pozwala uniknąć niepotrzebnego zajmowania miejsca w strukturach systemu przechowujących dane o procesach.

[wait.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/wait.c)

```c
int main ()
{
  pid_t pid;

  switch (pid = fork()) {                     /* powstaje nowy proces */

    case -1:                                  /* błąd funkcji fork() */
      syserr("Error in fork\n");              

    case 0:                                   /* proces potomny */

      printf("I am a child and my pid is %d\n", getpid());
      printf("I am a child and fork() return value is %d\n", pid);

      return 0;                               /* proces potomny kończy */

    default:                                  /* proces macierzysty */

      printf("I am a parent and my pid is %d\n", getpid());
      printf("I am a parent and fork() return value is %d\n", pid);

      if (wait(0) == -1) syserr("Error in wait\n");
					      /* czeka na zakończenie procesu potomnego */

      return 0;
  }
}
```

Aby zobaczyć proces zombie (oznaczony jako `<defunct>`), zastąp wywołanie `wait()` w procesie macierzystym przez `sleep(10)` a następnie wywołaj `./wait & ps`.


### 2.4 Uruchamianie nowych programów


Procesowi można zlecić wykonanie innego programu. Aktualnie wykonywany program zostanie wtedy zastąpiony innym. Służą do tego funkcje `exec()`. Jest 6 wersji tej funkcji różniących się sposobem podania argumentów.

Napisz `man 3 exec`, aby zobaczyć wszystkie możliwości. Krótkie wyjaśnienie:

- l — argumenty programu są w postaci listy napisów zakończonej `NULL`
- v — argumenty programu są w postaci tablicy napisów (tak jak `argv` dla funkcji `main`)
- p — ścieżka przeszukiwania plików wykonywalnych jest brana ze zmiennej środowiskowej `PATH`
- e — środowisko jest przekazywane jako ostatni parametr (rzadko używane)

Parametry:

- `path` — pełna ścieżka do wykonywalnego programu
- `file` — nazwa pliku z programem

Uwagi:

- pierwszy element na liście i w tablicy jest zawsze nazwą pliku zawierającego program, a dopiero następne elementy to właściwe argumenty programu
- jeżeli wykonanie funkcji `exec()` się powiedzie, to nigdy nie nastąpi powrót z jej wywołania
- funkcje `exec()` najczęściej wywołuje się zaraz po wywołaniu `fork()` w procesie potomnym


[exec.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/exec.c)

```c
int main ()
{
  pid_t pid;

  switch (pid = fork()) {                     /* tworzenie procesu potomnego */
    case -1: 
      syserr("Error in fork\n");

    case 0:                                   /* proces potomny */

      printf("I am a child and my pid is %d\n", getpid());      

      execlp("ps", "ps", NULL);               /* wykonuje program ps */
      syserr("Error in execlp\n");

    default:                                  /* proces macierzysty */

      printf("I am a parent and my pid is %d\n", getpid());

      if (wait(0) == -1) syserr("Error in wait\n");
					      /* czeka na zakończenie potomka */
      return 0;
  } 
}
```

Spróbuj uruchomić w ten sposób inne programy lub dodaj opcje do polecenia `ps`.


### 2.5 Kończenie procesu

Proces kończy działanie przez wywołanie funkcji `exit(int kod)`. W przypadku poprawnego zakończenia kod zakończenia powinien być równy 0, a różny od 0, jeżeli nastąpił błąd.


## 3 Ćwiczenie punktowane

W pliku [tree.c](https://github.com/Emilo77/SEM5-PW-LAB07/blob/master/tree.c) znajdziesz przykład drzewa procesów, w którym proces macierzysty tworzy `NR_PROC` procesów potomnych. Napisz program `line.c` tworzący linię składającą się z `NR_PROC` procesów, w której każdy proces potomny jest przodkiem następnego procesu.

Każdy proces powinien zaczekać na zakończenie potomka, o ile go posiada.

Wszystkie procesy oprócz pierwszego powinny wypisać na standardowe wyjście swój identyfikator a także identyfikator swojego rodzica.

Do obsługi błędów należy użyć funkcji `syserr()`.


## 4 Bibliografia

- man do poszczególnych funkcji systemowych
- W. R. Stevens "Programowanie zastosowań sieciowych w systemie UNIX", rozdz. 2.5.1-2.5.4
- M. J. Bach "Budowa systemu operacyjnego UNIX", rozdz. 7.1, 7.3-7.5
