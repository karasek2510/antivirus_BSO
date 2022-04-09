# Projekt BSO - Temat 1 - Antywirus
Szymon Kasperek

### Wstęp
Celem pierwszej części projektu było przygotowanie prostego antywirusa dla systemu z jądrem Linux. Program został napisany i testowany na ubuntu-20.04.3-desktop-amd64.
## Zaimplementowane funkcje:
### 1. Skanowanie pliku lub katalogu rekurencyjnie
Jako podstawową funkcje program umożliwia skanowanie pliku lub katalogu. Skanowanie uruchamia się za pomocą podkomendy "scan", dodatkowo należy wskazać cel (plik lub katalog) oraz plik przechowujący hashe (każdy kolejny w nowej linii). Mechnizm skanowania dokonuje wstępnej analizy pliku, aby odrzucić pliki problematyczne w odczycie w celu obliczenia hasha (pliki typu block, character itd). Dodatkowo sprawdzany system plików w jakim znajduje się skanowany plik - w przypadku innego niż EXT4, EXT3 i EXT2 plik jest ignorowany. Jeżeli obliczony znajduje się w pliku z bazą hashy nakładana jest kwarantanna.
### 2. Mechanizm kwarantanny
Kwarantanna polega na zaszyfrowaniu pliku (AES CFB) oraz przeniesieniu go do odpowiedniego katalogu znajdującego się w katalogu domowym użytkownika. Program dba o nałożenie uprawnień uniemożliwiający odczyt oraz zapis plików znajdujących się w katalogu z kwarantanną. Każdemu zaszyfrowanemu plikowi generowany jest ukryty plik o rozszerzeniu ".info" przechowujący informację o kwarantannie (nazwa pliku, oryginalna lokalizacja, uprawnienia w chwili nałożenia kwarantanny, klucz oraz wektor inicjalizujący, czas nałożenia kwarantanny). Wadą takiego rozwiązania jest to użytkownik (oraz root) jest w stanie ręcznie zmienić uprawnienia swojego katalogu kwarantanny oraz rozszyfrować plik po odczytaniu odpowiedniego klucza oraz wektora inicjalizującego. Rozwiązaniem jest stworzenie wspólnej kwaratanny dla wszystkich użytkowników oraz nadanie programowi SUID lub odpowiednich capabilities -  w takim przypadku jedynie root mógłby w niekontrolowany sposób dostać do katalogu kwarantanny.
### 3. Możliwość przywrócenia pliku z kwarantanny
Przywrócenie z kwarantanny polega na odszyfowaniu pliku, przeniesieniu go do oryginalnej lokalizacji oraz przywróceniu uprawnień. Po poprawnym przywróceniu pliki kwarantanny zostają usuniętę. Przywrócenie pliku wywołuje się za pomocą podkomendy "restore" oraz odpowiedniej nazwy pliku w kwarantannie (przekazywana jest podczas nakładania kwarantanny oraz można sprawdzić ją za pomocą podkomendy "showQuarantine").
### 4. Sprawdzenia jakie pliki znajdują się w kwarantannie
Umożliwia sprawdzenie podstawowych informacji o plikach znajdujących się w kwarantannie (nazwa pliku, oryginalna lokalizacja oraz czas nałożenia kwarantanny), które mogą być przypadtne przy identyfikacji pliku w przypadku chęci przywrócenia z kwaratanny. Sprawdzenie wywołuje się za pomocą podkomendy "showQuarantine".
### 5. Wyliczenie hasha wskazanego pliku
Wyliczenie hashy wywołuje się za pomocą podkomendy "hash" oraz wskazania na plik. Program zwraca hash MD5 w postaci wykorzystywanej przez moduł "scan" (dwa uint64_t w postaci decymalnej oddzielone przecinkiem).
## Wybór hasha oraz jego postać
Zdecydowałem się na wybór MD5 z uwagi na szybkość oraz popularność tego hasha. Większość bazy hashy wirusów posiada sygnatury między innymi w postaci MD5. W programie hash przetrzymywany jest w postaci tablicy dwóch liczb uint64_t. Taka postać pozwala na zaoszczędzenie połowy pamięci względem przechowywania w postaci stringa oraz zmniejszeniem ilości porównań w celu określenia czy dwa hashe są identyczne. Taka decyzja wiązała się z implementacją niestandardowego zapisu hasha aby uniknąć niepotrzebnych konwersji np. z postaci heksadecymalnej.

## Linkowanie
W projekcie zdecydowałem się na wykorzystanie dwóch bibliotek:
- crypto++ - funkcje kryptograficzne
- CLI11 - CLI parser

W przypadku crypto++ zdecydowałem się na statyczne linkowanie biblioteki z uwagi na jej niestandardowość i chęć uniknięcia problemów wynikających z jej braku w systmie operacyjnym.   
Biblioteka CLI11 w całości znajduje się w pliku CLI11.hpp i została dodana do projektu - jest jedna z zalecanych opcji w dokumentacji biblioteki.  
Reszta wykorzystywanych bibliotek jest standardowa i została zalinkowana dynamicznie.

## Uruchamienia i wyłączanie

Uruchamiania poszczególnych modułów programu zostało omówione w "Zaimplementowane funkcje" i sprowadza się do uruchomienia programu z odpowiednimi podkomendami, które można sprawdzić przez dopisanie flagi "-h" lub "--help".  
Aplikacja może zostać przerwana poprzez wciśnięcie kombinacji klawiszy "CTRL+C", która chwilowo zatrzymuje działanie programu i czeka na potwierdzenie chęci wyjścia z programu. W przypadku wciśnięcia "Y" program zostaje zakończony poprzez "return EXIT_FAILURE" w funkcji main.

## Uprawnienia użytkownika

Aplikacja może zostać uruchomiona przez każdego użytkownika i ma uprawnienia takie same jak on. W przypadku chęci skanowania plików niedostępnych dla zwykłego użytkonika należy uruchomić program z "sudo", aby otrzymał on uprawnienia roota i mógł odczytać oraz przekazać do kwaratanny większą ilość plików.

## Statystyki

Aplikacja wyświetla statystyki na temat wykonywanych akcji. Informuje o statusie przeskanowanych plików, postępie szyfrowania i deszyfrowania plików w kwarantannie itd. Na temat plików typu innego niż symlink oraz regular oraz te z nieobsługiwanego filesystemu nie są wyświetlane statystyki w celu zachowania przejrzystości.

## Stayczna oraz dynamiczna analiza kodu

Do napisania programu wykorzystywałem IDE z uruchomioną statyczną analizą kodu narzędziem clang-tidy. Zgodnie ze swoją wiedzą poprawiałem sugerowane przez narzędzie błędy oraz odnosiłem się do wskazanych uwag.  
Do analizy dynamicznej wykorzystałem narzędzie Valgrind w celu identyfikacji ewentualnych problemów z zarządzaniem pamięcią. 






