/*
** Przykladowa sesje pracy z modulem <param>.
**
** Dodanie definicji parametrow wywolania programu poporzez <param_add>.
** Przekazanie i zinterpetowanie parametrow wywolania programu poprzez <param_set>.
**  W tych etapach nalezy sprawdzic czy funkcje zwracaja <true> oznaczajace powodzenie wykonywanych czynnosci.
**
** Jesli wszystko przebieglo poprawnie mozna uzyskac dostep do zwalidowanych typow poprzez <param_get> lub dedykowane
**  funkcje <param_is_silent>, <param_is_verbose> i <param_is_help> dla parametrow wbudowanych.
**
** Bledy mozna czyscic za pomoca <param_clear>, zainicjowac parametry od nowa i sprobowac ponownie.
** Wyjscie moze byc sterowanie przez parametr <verbose> i <silent> przy uzyciu funkcji <param_cout>.
** Automatycznie generowana pomoc moze byc wypisana na wyjscie poprzez funkcje <param_show_help>.
*/

#include <inttypes.h>
#include <stdbool.h>

typedef enum{
    VAL_TYPE_BOOL,
    VAL_TYPE_STRING,
    VAL_TYPE_UINT8,
    VAL_TYPE_UINT16,
    VAL_TYPE_UINT32,
    VAL_TYPE_UINT64
}VAL_TYPE;

typedef struct{
    bool is_defined;
    bool is_specified;
    void * value;
    VAL_TYPE var_type;
    size_t var_size;
    char label[16];
}PARAM, *PPARAM;


/*
** Zostaja zdefiniowane parametry wywolania programu.
**
** Jest to wykonywane przez inicjacje wartosci zmiennych przechowujacej informacje o parametrze.
**
** dash_switch  -- etykieta (znak) odniesienia do parametru przy wywolaniu; jest z zakresu [a-z] i [A-Z]
** var_type     -- typ wartosci parametru
** name         -- etykieta (napis) odniesienia parametru przy dostepie w programie
**
** return       -- okresla wynik dzialania funkcji; gdy <false> -- oznacza niepowodzenie, inaczej <true>
**                  okresla powodzenie, nalezy sprawdzic kod bledu przez funkcje <param_get_err> w razie niepowodzenia
**
** Funkcja moze byc wywolana tylko przed wywolaniem funkcji <param_set> i/lub po <param_clear>.
** Zmienna <name> jest ograniczona do 16 znakow.
** Zmienna <dash_switch> nie moze przyjac wartosci {<s>, <v>, <h>}, poniewaz sa one zarezerwowane
**  dla wbudowanych parametrow wywolania. Wywola to blad.
**
** Zmienna <name> niezakonczona znakiem <\0> moze wywolac nieokreslone zachowanie programu.
*/
bool param_add(char dash_switch, VAL_TYPE var_type, char name[16]);


/*
** Interpetuje przekazane parametry wywolania programu. Parametry funkcji sa rownowazne
**  z parametrami glownej funkcji programu: <main(int argc, char **argv)>.
**
** Wartosci parametrow sa inicjowane. Nastepuje sprawdzanie wartosci pod wzgledem zainicjowanych
**  wczesniej typow.
**
** return   -- okresla wynik dzialania funkcji; gdy <false> -- oznacza niepowodzenie, inaczej <true>
**              okresla powodzenie, nalezy sprawdzic kod bledu przez funkcje <param_get_err> w razie niepowodzenia
**
** Moze byc wywolana tylko raz i kolejne uzycie zostaje umozliwione dopiero po wywolaniu funcji <param_clear>.
** Wymagane jest uzycie <param_clear> w celu deinizjalizacji zmiennych, po zakonczeniu uzuywania modulu <param.h>
**
** Typy liczbowe moga byc zainicjowane <0>, gdy wystapi blad konwersji. Jesli jest taka potrzeba
**  mozna sprawdzic kod bledu przez funkcje <param_get_err>.
*/
bool param_set(int argc, char **argv);


//#TODO niezainicjowane moga miec losowe wartosci
/*
** Zwraca wskaznik na wartosc parametru, ktory zostal zinterpretowany poprzez <param_set>.
**
** name     -- etykieta poszukiwanego parametru
**
** return   -- wskaznik na poszukiwana wartosc lub NULL gdy wartosc nie zostala zainicjalizowana podczas wywolania
**              funkcji <param_set>.
**
** Gdy etykieta nie zostala nadana poprzez funkcje <param_add> i gdy funkcja <param_set> nie zostala wogole wywolana, funkcja
**  zwraca kod bledu, ktory mozna sprawdzic poprzez wywolanie funkcji <param_get_err>.
**
** Zmienna <name> niezakonczona znakiem <\0> moze wywolac nieokreslone zachowanie programu.
*/
void * param_get(char name[16]);


/*
** Grupa funkcji zwracajaca wartosc wbudowanego parametru. Jest on typu bool.
**
** Sa rownowazne z wywolaniami funkcji:
**  -> param_get("silent");
**  -> param_get("verbose");
**  -> param_get("help");
*/
bool param_is_silent();
bool param_is_verbose();
bool param_is_help();


/*
** Zwalnia zaalokowana pamiec, czysci flage bledu, resetuje ustawienia nadane przez wywolanie funkcji <param_add>.
** Jest wymagana do zakonczenia uzycia modulu <param>.
*/
void param_clear();


/*
** Funkcja obudowujaca standardowe wyjscie.
**
** verbose      -- true gdy wyjscie ma byc uzyte tylko podczas podanego (wbudowanego) parametru verbose podczas wywolania
**               funkcji <param_set>, false gdy wyjscie ma byc uzyte zawsze z wyjatkiem padanego (wbudowanego) parametru silent
**               podczas wywolania funkcji <param_set>.
** format, ...  -- tak jak dla <printf>
*/
void param_cout(bool verbose, const char * format, ...);

/*
** Zwraca kod bledu.
**
** return   -- kod bledu w zakresie od 0 do 255.
*/
uint8_t param_get_err();

/*
** Wypisuje na standardowe wyjscie objasnienie parametrow wywolania.
*/
void param_show_help();
