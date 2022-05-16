# Projekt BSO - Temat 1 - Antywirus
Szymon Kasperek

# Część podstawowa

### Wstęp
Celem pierwszej części projektu było przygotowanie prostego antywirusa dla systemu z jądrem Linux. Program został napisany i testowany na ubuntu-20.04.3-desktop-amd64.
## Zaimplementowane funkcje:

### 1. Skanowanie pliku lub katalogu rekurencyjnie
Jako podstawową funkcję program umożliwia skanowanie pliku lub katalogu. Skanowanie uruchamia się za pomocą podkomendy "scan", dodatkowo należy wskazać cel (plik lub katalog) oraz plik przechowujący hashe (każdy kolejny w nowej linii). Mechanizm skanowania dokonuje wstępnej analizy pliku, aby odrzucić pliki problematyczne w odczycie w celu obliczenia hasha (pliki typu block, character itd). Dodatkowo sprawdzany system plików, w jakim znajduje się skanowany plik - w przypadku innego niż EXT4, EXT3 i EXT2 plik jest ignorowany. Jeżeli obliczony znajduje się w pliku z bazą hashy nakładana jest kwarantanna.

### 2. Mechanizm kwarantanny
Kwarantanna polega na zaszyfrowaniu pliku (AES CFB) oraz przeniesieniu go do odpowiedniego katalogu znajdującego się w katalogu domowym użytkownika. Program dba o nałożenie uprawnień uniemożliwiający odczyt oraz zapis plików znajdujących się w katalogu z kwarantanną. Każdemu zaszyfrowanemu plikowi generowany jest ukryty plik o rozszerzeniu ".info" przechowujący informację o kwarantannie (nazwa pliku, oryginalna lokalizacja, uprawnienia w chwili nałożenia kwarantanny, klucz oraz wektor inicjalizujący, czas nałożenia kwarantanny). Wadą takiego rozwiązania jest to użytkownik (oraz root) jest w stanie ręcznie zmienić uprawnienia swojego katalogu kwarantanny oraz rozszyfrować plik po odczytaniu odpowiedniego klucza oraz wektora inicjalizującego. Rozwiązaniem jest stworzenie wspólnej kwarantanny dla wszystkich użytkowników oraz nadanie programowi SUID lub odpowiednich capabilities - w takim przypadku jedynie root mógłby w niekontrolowany sposób dostać do katalogu kwarantanny.

### 3. Możliwość przywrócenia pliku z kwarantanny
Przywrócenie z kwarantanny polega na odszyfrowaniu pliku, przeniesieniu go do oryginalnej lokalizacji oraz przywróceniu uprawnień. Po poprawnym przywróceniu pliki kwarantanny zostają usunięte. Przywrócenie pliku wywołuje się za pomocą podkomendy "restore" oraz odpowiedniej nazwy pliku w kwarantannie (przekazywana jest podczas nakładania kwarantanny oraz można sprawdzić ją za pomocą podkomendy "showQuarantine").

### 4. Sprawdzenia jakie pliki znajdują się w kwarantannie
Umożliwia sprawdzenie podstawowych informacji o plikach znajdujących się w kwarantannie (nazwa pliku, oryginalna lokalizacja oraz czas nałożenia kwarantanny), które mogą być przypadatne przy identyfikacji pliku w przypadku chęci przywrócenia z kwarantanny. Sprawdzenie wywołuje się za pomocą podkomendy "showQuarantine".

### 5. Wyliczenie hasha wskazanego pliku
Wyliczenie hashy wywołuje się za pomocą podkomendy "hash" oraz wskazania na plik. Program zwraca hash MD5 w postaci heksadecymalnej.

## Wybór hasha oraz jego postać
Zdecydowałem się na wybór MD5 z uwagi na szybkość oraz popularność tego hasha. Większość bazy hashy wirusów posiada sygnatury między innymi w postaci MD5. W programie hash przetrzymywany jest w postaci tablicy dwóch liczb uint64_t. Taka postać pozwala na zaoszczędzenie połowy pamięci względem przechowywania w postaci stringa oraz zmniejszeniem ilości porównań w celu określenia czy dwa hashe są identyczne.

## Linkowanie
W projekcie zdecydowałem się na wykorzystanie dwóch bibliotek:

- crypto++ - funkcje kryptograficzne
- CLI11 - CLI parser

W przypadku crypto++ zdecydowałem się na statyczne linkowanie biblioteki z uwagi na jej niestandardowość i chęć uniknięcia problemów wynikających z jej braku w systemie operacyjnym.  
Biblioteka CLI11 w całości znajduje się w pliku CLI11.hpp i została dodana do projektu - jest jedna z zalecanych opcji w dokumentacji biblioteki.  
Reszta wykorzystywanych bibliotek jest standardowa i została za linkowana dynamicznie.

## Uruchamienia i wyłączanie

Uruchamiania poszczególnych modułów programu zostało omówione w "Zaimplementowane funkcje" i sprowadza się do uruchomienia programu z odpowiednimi podkomendami, które można sprawdzić przez dopisanie flagi "-h" lub "--help".  
Aplikacja może zostać przerwana poprzez wciśnięcie kombinacji klawiszy "CTRL+C", która chwilowo zatrzymuje działanie programu i czeka na potwierdzenie chęci wyjścia z programu. W przypadku wciśnięcia "Y" program zostaje zakończony poprzez "return EXIT_FAILURE" w funkcji main.

## Uprawnienia użytkownika

Aplikacja może zostać uruchomiona przez każdego użytkownika i ma uprawnienia takie same jak on. W przypadku chęci skanowania plików niedostępnych dla zwykłego użytkownika należy uruchomić program z "sudo", aby otrzymał on uprawnienia roota i mógł odczytać oraz przekazać do kwarantanny większą ilość plików.

## Statystyki

Aplikacja wyświetla statystyki na temat wykonywanych akcji. Informuje o statusie przeskanowanych plików, postępie szyfrowania i deszyfrowania plików w kwarantannie itd. Na temat plików typu innego niż symlink oraz regular, oraz te z nieobsługiwanego filesystemu nie są wyświetlane statystyki w celu zachowania przejrzystości.

## Statyczna oraz dynamiczna analiza kodu

Do napisania programu wykorzystywałem IDE z uruchomioną statyczną analizą kodu narzędziem clang-tidy. Zgodnie ze swoją wiedzą poprawiałem sugerowane przez narzędzie błędy oraz odnosiłem się do wskazanych uwag.  
Do analizy dynamicznej wykorzystałem narzędzie Valgrind w celu identyfikacji ewentualnych problemów z zarządzaniem pamięcią.

# Część zaawansowana

## Dodatkowa funkcja - wsparcie YARA

Jako dodatkową funkcję postanowiłem zaimplementować możliwość skanowania plików za pomocą mechanizmu YARA. Dodatkowy moduł o nazwie "yara" pozwala w sposób analogiczny do modułu scan dokonywać analizy wskazanych plików i katalogów. Zamiast bazy skrótów plików uznawanych za niebezpieczne należy przekazać jeden lub wiele plików zawierających odpowiednio sformatowane reguły zgodne ze standardem wykorzystywanym przez narzędzie YARA. W przypadku dopasowania z co najmniej jedną z reguł następuje nałożenia kwarantanny w ten sam sposób co podczas normalnego skanowania z wykorzystaniem hashy.  
W swojej implementacji wykorzystałem elementy kodu źródłowego z repozytorium yaracpp (wrappera biblioteki yara dla języka C++).  
[Link do repozytorium](https://github.com/avast/yaracpp)

## Działanie w tle - ciągłe skanowanie z wykorzystaniem inotify

W ramach funkcji oferującej działanie w tle zaimplementowałem moduł nadzorujący katalog (oraz kolejne w sposób rekurencyjny). W przypadku modyfikacji lub utworzenia pliku dokonywane jest skanowanie z wykorzystaniem skrótów plików. Moduł wykorzystuje wielowątkowość w celu zapewnienia ciągłości otrzymywania informacji o zdarzeniach związanych z system plików. W przypadku zaistnienia takiego zdarzenia tworzony jest nowy wątek, którego zadaniem jest przeskanowanie pliku i ewentualne nałożenie kwarantanny.  
W celu ograniczenia ilości wątków podczas wywoływania programu z takim modułem należy przekazać liczbę maksymalnej ilości wątków działających jednocześnie.  
Chcąc zakończyć monitorowanie, należy wpisać znak "q" co pozwoli zakończyć się wątkom monitorującym i pozwoli niesiłowo zakończyć program. 






