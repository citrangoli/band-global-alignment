# Band Global Alignment
### Barbera Thomas 845538

Questo programma riceve in input due sequenze definite nell'alfabeto degli amminoacidi e ne calcola l'allineamento globale ottimo utilizzando BLOSUM62 come matrice di score.

Il software è stato scritto e testato tramite gcc v9.3.0.

L'input deve essere fornito tramite un file presente nella stessa cartella dell'eseguibile denominato `input.txt`. Esso deve contenere quattro righe:
* La prima riga deve contenere la lunghezza della prima sequenza.
* La seconda riga deve contenere la lunghezza della seconda sequenza.
* La terza e quarta riga contengono le sequenze vere e proprie. Esse devono essere indicate nell'alfabeto degli amminoacidi e scritte in maiuscolo. Se non vengono rispettate le seguenti condizioni il software può generare errori.

L'output permette di visualizzare le due stringhe allineate (con l'introduzione degli indel '-') e permette inoltre di valutare visivamente l'allineamento tramite una terza stringa composta come segue:
* Ogni carattere viene calcolato sulla base dei caratteri incolonnati corrispondenti nell'allineamento delle due stringhe.
* Il simbolo '#' indica un match esatto.
* Il simbolo '-' indica la presenza di un indel in una delle due stringhe.
* Il simbolo 'O' identifica due caratteri differenti senza la presenza di un indel.

Vengono inoltre resi disponibili in output la lunghezza dell'allineamento e lo score ottenuto.
