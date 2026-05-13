#  Progetto finale (bozza n. 2)

Il progetto richiede di lavorare un un grafo non orientato, pesato. Un generico arco deve essere rappresentata dalla struttura:

```c++
typedef struct arco {
    int u, v;
    int weight;
    bool msf;
    struct arco *next;
} arco;
```

Dato che il grafo non è orientato e non ha self-loop avremo sempre `u < v`. Il peso `weight` può essere arbitrario (anche negativo). Il significato degli altri campi verrà descritto successivamente. Se necessario nella `struct` potete inserire anche altri campi.

La prima parte del progetto consiste nello scrivere un programma C per calcolo della minimum spanning forest con l'algoritmo di Kruskal. Per la gestione delle componenti connesse utilizzare una Union Find data structure utilizzando per maggiore efficienza le tecniche di path compression e Union by rank.

Al termine di questa prima computazione il programma deve costruire una rappresentazione del grafo consistente in 3 distinte strutture: l'insieme degli archi, le liste di adiacenza dei singoli nodi, la tabella delle componenti connesse. Nel seguito si userà l'abbreviazione MSF per denotare la minimum spanning forest e la scrittura `(u,v,w)` per indicare l'arco non orientato di peso `w` che collega i nodi `u` e `v`.

## Rappresentazione del grafo

* **L'insieme degli archi**: deve essere rappresentato mediante una tabella hash `gHash` (con collisioni gestite mediante liste concatenate) contenente tutti gli archi del grafo rappresentati dalla struct `arco`. Il campo `msf` deve essere `true` per tutti e soli gli archi della minimum spanning forest, e il campo `next` deve essere usato per le liste concatenate della tabella hash.
* **Le liste di adiacenza**: devono essere rappresentate mediante un array `vicini` di lunghezza pari al numero dei nodi tale che `vicini [i]` è la lista (solita linked list) contenente tutti i nodi adiacenti al nodo `i` ordinati per indice crescente. Ad esempio se nel grafo sono contenuti gli archi `(u,v,w)`:
    * 4 10 6
    * 10 17 8
    * 2 10 4
    * 10 15 3
    * 10 21 7
      allora la lista `vicini [10]` deve contenere nell'ordine gli elementi `2 4 15 17 21` cioè i nodi adiacenti al nodo 10, ordinati per indice crescente. Naturalmente a sua volta il 10 comparirà nelle liste di adiacenza dei nodi 2, 4, etc. Nelle liste di adiacenza oltre all'identificatore deve essere contenuto il peso dell'arco e un boolean che indica se appartiene alla MSF. Di conseguenza gli elementi delle lista di adiacenza dovranno avere una struttura del tipo:
```c++
typedef struct elemento {
    int id; // indice del nodo
    int w; // peso
    bool msf; // true sse questo arco appartiene alla MSF
    struct elemento *next;
} elemento;
```
Si noti che un generico arco `(u,v,w)` appare in tre posti diversi: nella tabella hash `gHash` nella lista `vicini[u]` e nella lista `vicini[v]`.
* **La tabella delle componente connesse**: deve essere mantenuta in un array `cCon` di lunghezza pari al numero dei nodi tale che `cCon[i]` contiene l'identificatore della componente connessa a cui appartiene il nodo `i`. L'identificatore di una componente connessa è semplicemente l'indice del nodo più piccolo in essa contenuto. Ad esempio, in un grafo di 10 nodi le cui quattro componenti connesse sono: `0 4 7 9`, `8 6`, `5 2 1`, `3`, gli identificatori delle componenti sono 0, 6, 1, e 3 e l'array `cCon` deve contenere i valori: `0 1 1 3 0 1 6 0 6 0`.

Per semplicità è opportuno che queste tre strutture dati siano riunite in una unica struct:

```c++
typedef struct {
    arco **gHash;      // tabella hash (array di liste di archi)
    elemento **vicini; // array di liste di adiacenza
    int *cCon;         // array delle componenti connesse
    int numCoCo;       // numero di componente connesse
    long costoMSF;     // costo della MSF
} grafo;
```


Si noti che malgrado gli identificatori degli archi e dei pesi possono essere rappresentati con un `int` (32 bit), per il costo della MSF è necessario usare un `long` in quanto ci sono istanze in cui tale costo è maggiore di 2^31. Oltre a questi campi ne dovranno essere inseriti altri per gestire le operazioni in multithreading.

---

## Operazioni da supportare

Il progetto deve contenere le funzioni in grado di aggiornare le tre strutture dati viste sopra a seguito delle due seguenti operazioni.

### Aggiungi arco (u,v,w) [solo progetto completo]

Dato l'arco `(u,v,w)` se i due nodi `u` e `v` sono in componenti diverse della MSF l'arco viene aggiunto alla MSF. Le operazioni da fare sono quindi:
* aggiunta di `(u,v,w)` alla tabella hash `gHash`
* aggiunta di `u` alla lista `vicini [v]` e di `v` alla lista dei `vicini [u]`
* aggiornamento dell'array `cCon` delle componenti connesse: i nodi di una delle componenti connesse devono infatti ricevere un nuovo identificatore.

Se invece i due nodi `u` e `v` sono nella stessa componente connessa C, dopo aver aggiunto l'arco si deve verificare se il nuovo arco deve essere inserito nella MSF eseguendo le seguenti operazioni:
* si cerca l'unico cammino che collega `u` e `v` all'interno dell'albero che ricopre la componente C (si tratta di un cammino formato tutti di archi della MSF, si utilizzi `vicini` per costruirlo ma considerando solo gli archi con `msf==true`)
* all'interno di tale cammino si cerca l'arco `(x,y,v)` di peso massimo: se `v > w` allora si elimina `(x,y,v)` dalla MSF e si aggiunge `(u,v,w)`.

Rispetto al caso precedente l'array `cCon` non deve essere modificato, ma attenzione che per eliminare `(x,y,v)` dalla MSF il flag `msf` deve essere posto a `false` in tutte le rappresentazioni dell'arco.

### Cancella arco (u,v,w)

Se l'arco `(u,v,w)` non è nella MSF si cancella dal grafo (aggiornando `gHash` e `vicini`) e non ci sono altre operazioni da fare. Se l'arco è nella MSF lo si cancella dal grafo ma come effetto di questo la componente C contente `u` e `v` potrebbe essere spezzata in due componenti diverse. Si proceda nel seguente modo:
* si esegue una visita della componente contenente `u` e si costruisce la lista `Lu` dei nodi dell'albero di copertura raggiungibili da `u` (e senza passare da `v` dato che l'arco `(u,v)` è stato tolto)
* si costruisce una analoga lista `Lv` dei nodi della componente raggiungibili da `v`
* sempre utilizzando le liste di adiacenza `vicini` si cerca l'arco di costo minimo che collega un nodo in `Lu` con un nodo di `Lv`. Tale arco viene aggiunto alla MSF .
* Se non esistono archi che collegano `Lu` con `Lv` allora queste diventano due componenti connesse diverse ed è necessario aggiornare l'array `cCon`.

Al termine di entrambe le operazioni di modifica, oltre alle strutture dati devono essere aggiornati i campi `costoMSF` e `numCoCo` con i nuovi valori del costo della MSF e il numero delle componenti connesse.

---

## Gestione di aggiornamenti concorrenti

Le operazioni di cancellazione e inserimento degli archi sono fatti in maniera concorrente da thread diversi. Per evitare race conditions è necessario usare dei meccanismi di sincronizzazione. A questo scopo si utilizzino dei mutex; idealmente si potrebbe usare:
* un mutex per ogni posizione della tabella hash `gHash`: quando si deve inserire/cancellare un arco `(u,v,w)` che si trova nella lista `gHash[i]` prima di accedere alla lista si fa un lock al relativo mutex.
* un mutex per ogni componente connessa: prima di fare un'operazione su un arco `(u,v,w)` deve essere fatto un lock sui mutex delle componenti connesse (può essere anche una sola) a cui appartengono i nodi `u` e `v`.

Dato però che non è pratico usare un numero elevato di mutex, si utilizzi la seguente strategia: si utilizzino due array `mut_gHash` e `mut_cCon` di `nmutex` elementi; quando si deve accedere a `gHash [i]` si effettua un lock su `mut_gHash [i%nmutex]` e quando si deve accedere alla componente connessa i-esima si effettua un lock su `mut_cCon[i%nmutex]` . Per semplicità rendete gli array `mut_gHash` e `mut_cCon` e la variabile `nmutex` dei campi della struttura `grafo`.

## Gestione dei thread

Il thread principale legge le operazioni di inserimento/cancellazioni da fare da un file e svolge il ruolo di produttore in uno schema produttori/consumatori. I thread consumatori, il cui numero è specificato sulla linea di comando, leggono dal buffer produttori/consumatori la descrizione di una operazione e la eseguono in maniera concorrente.

## Avvio del programma

Il programma eseguibile si deve chiamare `msf.out` deve essere invocato con la seguente linea di comando:
`msf.out file_grafo file_archi [-t threads] [-H hashsize] [-M nmutex]`
dove `file_grafo` è il file contenente la descrizione del grafo, `file_archi` è il file contenente le descrizione delle operazioni di inserimento/cancellazione degli archi, il parametro `-t` indica il numero di threads consumatori (default 3 se `-t` non viene indicato), il parametro `-H` la dimensione della tabella hash (default 100K), il parametro `-M` il numero di mutex nelle tabelle `mut_gHash` e `mut_cCon` (default 1000). Per la gestione delle optioni sulla linea di comando utilizzare la funzione `getopt(3)`.

---

## Formato dei file di input

Il file contenente la descrizione del grafo sarà nel formato `.gr` descritto in questa pagina. Un esempio minimo di file `.gr` è il seguente:
```text
c Esempio minimo di formato .gr
p sp 6 7
c
c arco dal nodo 1 al nodo 2 di peso 17
a 1 2 17
c
a 1 3 10
a 2 4 2
a 3 5 0
a 4 6 3
a 2 5 1
a 5 6 20
```

Si noti che la linea `p sp` contiene il numero di nodi e di archi. Attenzione che nel file i nodi sono numerati a partire da 1: dato che per noi il nodo 0 è perfettamente valido il numero di nodi effettivi è uno in più di quello indicato. Potete assumere che il file di input contenga solamente archi con `u < v` e che non contenga archi duplicati.

Il file contenente le operazioni sui grafi ha il seguente formato: ogni linea deve iniziare con uno dei caratteri `+`, o `-`:
* se la linea ha la forma `- u v` (ad esempio `- 4 10`) indica un arco che deve essere eliminato
* se la linea ha la forma `+ u v w` (ad esempio `+ 3 9 5`) indica un arco da aggiungere

Le linee che non rispettano questo formato vanno ignorate. Potete assumere che in questo file gli archi siano sempre indicati con `u < v`.

---

## Esecuzione del programma

Inizialmente il programma deve calcolare la MSF con l'algoritmo di Kruskal e visualizzare su stdout tre interi che indicano rispettivamente: il numero totale di archi, il numero di componenti connesse, il costo della MSF. Nell'esempio minimo fornito sopra l'output deve essere `7 2 16` .

Successivamente il programma deve leggere il file delle operazioni e per ogni operazione scrivere l'esito:
* una terna con: archi, componenti connesse, e costo nuovo MSF se l'operazione era valida
* il valore `0` se l'operazione non era valida.

Il thread che gestisce l'operazione deve stampare su stdout con una singola `printf` la descrizione dell'operazione e l'esito. Esempio di output per le operazioni fornite:
```text
+ 0 2 7 8 1 23
+ 4 6 1 0
- 5 6 7 1 23
- 1 6 0
+ 3 9 2 0
- 2 4 6 2 21
- 3 5 5 2 38
```

Al termine della lettura delle operazioni i thread consumatori devono essere fatti terminare e il programma deve stampare su stdout la stringa "Operazioni terminate". Successivamente il programma deve ricalcolare numero archi, costo MSF e componenti connesse facendo una scansione completa delle strutture e stampare la terna finale.

---

##  Requisiti fondamentali

* non si devono usare variabili globali
* tutte le allocazioni devono essere fatte dinamicamente mediante `malloc/realloc`
* tutta la memoria deve essere correttamente deallocata (verificare con `valgrind`). Per evitare problemi con `pthread_exit`, terminare il thread con `return NULL`.
* in caso di errori il programma deve terminare con un messaggio esplicativo su `stderr` e con un exit code diverso da zero
* il programma deve usare `stderr` per messaggi di debug e `stdout` solamente per le informazioni indicate
* l'eseguibile deve chiamarsi `msf.out` e la compilazione deve avvenire con `make` (opzione `-O3`) .

---

## Implementazione python

Programma `msf.py` invocato come `msf.py file_grafo file_archi` . Legge il grafo e le operazioni e infine calcola la MSF utilizzando l'algoritmo di Jarník/Prim (da ripetere su tutti i nodi non coperti per gestire le varie componenti) . Stampa su stdout la terna finale.

## Implementazione java (solo progetto completo)

Programma `Msf` invocato come `java Msf file_grafo file_archi` . Si comporta come il programma python ma calcola la MSF con l'algoritmo di Kruskal. Stampa su stdout la terna finale.

## Consegna del progetto

Esclusivamente mediante GitHub privato. Aggiungere come collaboratore l'utente `Laboratorio2B`. Caricare l'URL per la clonazione su Moodle (forma `git@github.com:user/progetto.git`) . Non fare commit dopo la data di consegna. Nel repository mettere solo i file essenziali e un `README.md` dettagliato .

## Test del codice

Utilizzo delle GitHub Actions controllate dal file `ci.yml` e dai file in `msf.zip` . Scompattare `msf.zip` nella root del progetto senza modificare i file contenuti . I risultati dei test saranno visibili nel tab Actions di GitHub.