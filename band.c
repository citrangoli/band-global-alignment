// Barbera Thomas 845538 - Band Global Alignment

#include "band.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

bool readFile(char** str1, char** str2) {
	/*
	Legge il file di input che contiene le sequenze precedute dalle loro
	lunghezze, come da specifica. Memorizza quindi le due sequenze in
	str1 e str2. La funzione ritorna true se è stato possibile leggere il
	file e false se ci sono stati degli errori (ad esempio se il file
	non viene trovato).
	*/

	int len1, len2;
	FILE *input;
	if ((input = fopen("input.txt", "r")) == NULL)
		return false;

	fscanf(input, "%d", &len1);
	fscanf(input, "%d", &len2);
	// Viene allocata la memoria necessaria a contenere le due stringhe
	// da leggere dal file. La lunghezza è aumentata di uno per poter
	// contenere anche l'ultimo carattere di terminazione stringa.
	*str1 = malloc((len1+1) * sizeof(char));
	*str2 = malloc((len2+1) * sizeof(char));
	fscanf(input, "%s", *str1);
	fscanf(input, "%s", *str2);

	fclose(input);
	return true;
}

int aminoacidToIndex(char aminoacid) {
	/*
	Dato in input un amminoacido, restituisce il suo indice all'interno
	della matrice Blosum62.
	Si assume che il carattere passato come input appartenga all'alfabeto
	degli amminoacidi (e che sia una lettera maiuscola). In caso contrario
	si ottengono errori.
	*/

	char aminoacidsList[24] = {'A', 'R', 'N', 'D', 'C', 'Q', 'E', 'G', 'H', 'I', 'L', 'K', 'M', 'F', 'P', 'S', 'T', 'W', 'Y', 'V', 'B', 'Z', 'X', '*'};
	int i = 0;
	while (aminoacid != aminoacidsList[i]) i++;
	return i;
}

bool validPosition(int mrows, int mcols, int base, int k, int r, int c) {
	/*
	Verifica che una certa coordinata/cella della matrice sia 'valida',
	cioè appartenente alla banda. Se la cella fuoriesce dalla matrice
	o dalla banda viene restituito false, altrimenti true.
	Input di questa funzione (nell'ordine): numero di righe della matrice di
	prog dinamica, numero di colonne della matrice di prog dinamica, base
	della banda, incremento k della banda, riga e colonna della cella di cui
	verificare la validità.
	*/

	if (r < 0 || r > mrows-1) return false;
	if (c < 0 || c > mcols-1) return false;
	// La cella appartiene alla matrice, ora si verifica se appartiene
	// alla banda. Per ogni riga della matrice solo alcune colonne
	// sono valide (il che dipende dalla base e da k).
	int minValidCol = r - k;
	int maxValidCol = r + base - 1 + k;
	return !(c < minValidCol || c > maxValidCol);
}

bool nextCell(int mrows, int mcols, int base, int k, int *nr, int *nc) {
	/*
	Questa funzione viene usata durante la scansione e scrittura della matrice
	di programmazione dinamica per calcolare la coordinata della prossima cella
	da visitare. Riceve in input le dimensioni della matrice, i parametri della
	banda e le coordinate della cella attuale (da aggiornare con quelle della
	prossima cella da visitare). La strategia è di scansionare la matrice da sinistra
	a destra e dall'alto al basso ma saltando tutte le celle che non appartengono
	alla banda (altrimenti perderemmo il vantaggio dell'approccio "a banda").
	Questa funzione permette di effettuare questo "salto" in tempo costante.
	Il valore booleano di output informa se è stato possibile trovare la "next cell"
	o meno (in pratica verrà ritornato false quando la scansione sarà finita,
	altrimenti sarà restituito true).
	*/

	// Memorizzo le vecchie coordinate:
	int pr = *nr;
	int pc = *nc;

	if (validPosition(mrows, mcols, base, k, pr, pc+1)) {
		*nr = pr;
		*nc = pc+1;
		return true;
	}
	if (pr+1 > mrows-1) return false;
	*nr = pr+1;
	if (pr+1-k < 1) *nc = 1;
	else *nc = pr+1-k;
	return true;
}

int coordConv(int base, int k, int r, int c) {
	/*
	Questa funzione converte le coordinate bidimensionali della matrice di
	programmazione dinamica in un indice monodimensionale (usato per
	accedere all'array che contiene i soli dati dentro la banda della matrice).
	Riceve in input le dimensioni della banda (base e k) e la riga e colonna
	da convertire. Restituisce in output l'indice nell'array di banda.
	Si assume che l'input della funzione sia corretto, in particolare la riga
	e la colonna da convertire devono appartenere veramente alla banda.
	*/

	int width = base + 2*k;
	return width*r + c - r + k;
}

void reverseString(char *s, int begin, int end) {
	/*
	Riceve in input una stringa e la specchia con un approccio ricorsivo.
	A ogni passo della ricorsione vengono scambiati i due caratteri indicati da
	begin e end. Essi partono dall'esterno per poi muoversi verso l'interno della
	stringa.
	*/

	if (begin >= end) return;

	char temp;
	temp = *(s+begin);
	*(s+begin) = *(s+end);
	*(s+end) = temp;

	reverseString(s, ++begin, --end);
}

int main() {
	// Dichiarazione delle due stringhe in input.
	char* s1 = NULL;
	char* s2 = NULL;

	// Il primo passaggio è leggere le sequenze in input.
	if (!readFile(&s1, &s2)) {
		// Il file non è stato letto correttamente, il programma termina.
		printf("Errore nella lettura del file. Terminazione.\n");
		return -1;
	}

	// È utile che s1 sia la più corta delle due, per questo le
	// stringhe vengono eventualmente scambiate.
	if (strlen(s1) > strlen(s2)) {
		char *temp = s1;
		s1 = s2;
		s2 = temp;
	}

	// Vengono calcolate le lunghezze delle stringhe, la base della banda
	// e l'incremento k. Da notare la base che è calcolata sommando uno alla
	// differenza delle lunghezze delle stringhe. Ciò permette di poter arrivare
	// dalla cella in alto a sinistra a quella in basso a destra nella prima
	// iterazione con un k = 0.
	size_t l1 = strlen(s1);
	size_t l2 = strlen(s2);
	size_t base = l2 - l1 + 1;
	int k = 0;

	// Vengono ora dichiarati gli array che conterranno i valori appartenenti alle
	// bande delle matrici di programmazione dinamica e di traceback.
	// Gli array verranno allocati della lunghezza corretta ad ogni passo
	// di computazione, man mano che la banda si espande.
	// Il numero di righe e colonne delle "matrici" rappresentate sarà maggiore
	// di uno rispetto alla lunghezza delle stringhe in quanto la riga zero e
	// la colonna zero tengono in considerazione il caso in cui una delle
	// due stringhe è vuota.
	int* dpArray = NULL;
	char* tracebackArray = NULL;

	// Le seguenti variabili sono necessarie ai conteggi degli allineamenti durante
	// le scansioni della matrice di programmazione dinamica.
	int r, c;
	int leftValue, upValue, diagonalValue, maxValue;
	char direction;
	int maxAlignmentValueOutsideBand;
	int latestK;
	bool lastIteration = false;

	// Il seguente ciclo itera la computazione aumentando il valore di k.
	// L'iterazione si ferma quando il valore di k porta alla copertura dell'intera
	// matrice o quando lo score dell'allineamento è maggiore di quello massimo
	// raggiungibile con un percorso che esce dalla banda.
	while (!lastIteration) {
		// Viene calcolato il numero di celle appartenenti alla banda in
		// modo da allocare la corretta quantità di memoria.
		// Viene quindi allocato lo spazio necessario in due array (conterranno
		// solo celle appartenenti alla banda).
		int arrayLength = (base+2*k) * (l1+1);
		dpArray = (int*) realloc(dpArray, arrayLength*sizeof(int));
		tracebackArray = (char*) realloc(tracebackArray, arrayLength*sizeof(char));
		latestK = k;

		// Vengono ora inizializzate le celle corrispondenti alla riga zero e
		// alla colonna zero delle matrici di programmazione dinamica e traceback.
		// Nel caso della matrice di programmazione dinamica, la riga e la colonna
		// zero sono calcolate nel seguente modo in quanto rappresentano
		// un allineamento tra due stringhe in cui una è un prefisso di s1 o s2
		// mentre l'altra è composta da zero caratteri. Per effettuare tale
		// allineamento è necessario che questa seconda stringa venga riempita di
		// indel per far combaciare la sua lunghezza con quella della prima stringa.
		// Un indel incolonnato con qualsiasi altro amminoacido ha un peso di -4, quindi
		// il valore da inserire nella cella è la lunghezza della stringa moltiplicata
		// per -4. La cella (0, 0) ha come valore speciale zero, in quanto
		// rappresenta un allineamento tra due stringhe vuote.
		// Per quanto riguarda la matrice traceback, invece, la riga e la colonna zero
		// sono inizializzate per portare alla cella (0, 0) che termina il calcolo del
		// percorso di traceback (ha un carattere speciale che verrà riconosciuto).
		// La colonna e la riga zero portano di fatto all'aggiunta di indel.
		// Naturalmente nell'array sono disponibili solo le celle che appartengono
		// alla banda, quindi esse sono le uniche celle "di bordo" ad essere inizializzate.
		// Partendo dalle celle a destra e sotto (0, 0) si seguono la riga 0 e la
		// colonna 0 rispettivamente fino al confine della banda (punto in cui ci si arresta).

		int bi;  // boundary index

		// Condizioni al bordo della matrice di programmazione dinamica:
		dpArray[coordConv(base, k, 0, 0)] = 0;
		bi = 1;
		while (bi<l1+1 && validPosition(l1+1, l2+1, base, k, bi, 0)) {
			dpArray[coordConv(base, k, bi, 0)] = (-4)*bi;
			bi++;
		}
		bi = 1;
		while (bi<l2+1 && validPosition(l1+1, l2+1, base, k, 0, bi)) {
			dpArray[coordConv(base, k, 0, bi)] = (-4)*bi;
			bi++;
		}

		// Condizioni al bordo della matrice di traceback:
		tracebackArray[coordConv(base, k, 0, 0)] = '#';
		bi = 1;
		while (bi<l1+1 && validPosition(l1+1, l2+1, base, k, bi, 0)) {
			tracebackArray[coordConv(base, k, bi, 0)] = 'U';
			bi++;
		}
		bi = 1;
		while (bi<l2+1 && validPosition(l1+1, l2+1, base, k, 0, bi)) {
			tracebackArray[coordConv(base, k, 0, bi)] = 'L';
			bi++;
		}

		// Viene settato il punto di partenza per la scansione,
		// cioè il punto (1, 1):
		r=1; c=1;
		// Viene fatta partire la scansione della banda:
		do {
			maxValue = INT_MIN;
			// Verifica del valore a SINISTRA:
			if (validPosition(l1+1, l2+1, base, k, r, c-1)) {
				leftValue = dpArray[coordConv(base, k, r, c-1)] + blosum62[aminoacidToIndex('*')][aminoacidToIndex(s2[c-1])];
				if (leftValue > maxValue) {
					maxValue = leftValue;
					direction = 'L';
				}
			}
			// Verifica del valore SOPRA:
			if (validPosition(l1+1, l2+1, base, k, r-1, c)) {
				upValue = dpArray[coordConv(base, k, r-1, c)] + blosum62[aminoacidToIndex(s1[r-1])][aminoacidToIndex('*')];
				if (upValue > maxValue) {
					maxValue = upValue;
					direction = 'U';
				}
			}
			// Verifica del valore DIAGONALE:
			if (validPosition(l1+1, l2+1, base, k, r-1, c-1)) {
				diagonalValue = dpArray[coordConv(base, k, r-1, c-1)] + blosum62[aminoacidToIndex(s1[r-1])][aminoacidToIndex(s2[c-1])];
				if (diagonalValue > maxValue) {
					maxValue = diagonalValue;
					direction = 'D';
				}
			}
			// Scrittura nelle matrici di programmazione dinamica e traceback
			// dei valori appena calcolati.
			dpArray[coordConv(base, k, r, c)] = maxValue;
			tracebackArray[coordConv(base, k, r, c)] = direction;
			// Con il while seguente vengono aggiornate le coordinate 'r' e 'c'
			// per poter leggere nell'iterazione successiva la prossima cella
			// della banda. Se non ci sono più celle nella banda il ciclo termina.
		} while (nextCell(l1+1, l2+1, base, k, &r, &c));

		// Se il valore di k è talmente alto da portare alla copertura totale della
		// matrice, significa che è stata eseguita l'ultima iterazione.
		lastIteration = k>=l2+1-base;

		// Prima di ampliare la banda, si può verificare se siamo sicuri di
		// aver già trovato l'allineamento ottimo. In tal caso possiamo interrompere
		// le iterazioni.
		// Il numero minimo di indel in un allineamento che esce dalla banda è
		// base-1+2k. Possiamo quindi sapere quale sarà al massimo lo score di
		// questo allineamento e confrontarlo con lo score attuale.
		// Questo limite superiore corrisponde a un allineamento dove tutti i
		// caratteri di s1 fanno un match e si ha il minore numero possibile di indel.
		maxAlignmentValueOutsideBand = 0;
		for (int i=0; i<l1; i++)
			maxAlignmentValueOutsideBand += blosum62[aminoacidToIndex(s1[i])][aminoacidToIndex(s1[i])];
		maxAlignmentValueOutsideBand += (-4 * (base-1+2*k));
		if (dpArray[coordConv(base, k, l1, l2)] >= maxAlignmentValueOutsideBand) break;

		// Viene incrementato k, allargando la banda:
		if (k==0) k=1;
		else k*=2;
	}

	// Si ripristina k all'ultimo valore usato per l'allocazione dei due array
	// dato che il valore è importante per il corretto funzionamento di coordConv().
	// Di fatto si vuole annullare un eventuale ultimo incremento.
	k = latestK;

	// L'allineamento è concluso, ora si deve applicare il traceback per ricostruirlo.
	// Verrà utilizzata la matrice che è stata usata per appuntare le direzioni
	// di spostamento e si ricostruiranno due stringhe con l'aggiunta degli indel.
	// Per prima cosa è necessario conoscere la lunghezza di tali stringhe per
	// poterle allocare (lunghezza allineamento), quindi verrà letta la matrice
	// di traceback una prima volta per effettuare il conteggio e una seconda per
	// costruire le stringhe.
	int alignmentLength = 0;
	r = l1;
	c = l2;
	while (tracebackArray[coordConv(base, k, r, c)] != '#') {
		alignmentLength++;
		if (tracebackArray[coordConv(base, k, r, c)] == 'L') c--;
		else if (tracebackArray[coordConv(base, k, r, c)] == 'U') r--;
		else if (tracebackArray[coordConv(base, k, r, c)] == 'D') {
			r--;
			c--;
		}
	}

	char aligned_s1[alignmentLength+1];
	char aligned_s2[alignmentLength+1];
	aligned_s1[alignmentLength] = '\0';
	aligned_s2[alignmentLength] = '\0';
	// Ora si possono generare le nuove stringhe con indel:
	int p = 0;
	r = l1;
	c = l2;
	while (tracebackArray[coordConv(base, k, r, c)] != '#') {
		if (tracebackArray[coordConv(base, k, r, c)] == 'L') {
			aligned_s1[p] = '-';
			aligned_s2[p] = s2[c-1];
			c--;
		}
		else if (tracebackArray[coordConv(base, k, r, c)] == 'U') {
			aligned_s1[p] = s1[r-1];
			aligned_s2[p] = '-';
			r--;
		}
		else if (tracebackArray[coordConv(base, k, r, c)] == 'D') {
			aligned_s1[p] = s1[r-1];
			aligned_s2[p] = s2[c-1];
			r--;
			c--;
		}
		p++;
	}

	// Le stringhe che sono state costruite sono state lette al contrario,
	// quindi è necessario effettuare il reverse.
	reverseString(aligned_s1, 0, strlen(aligned_s1)-1);
	reverseString(aligned_s2, 0, strlen(aligned_s2)-1);

	// Calcoliamo ora una terza stringa di paragone tra le prima due.
	// Verrà usata per dare un output "visvo" della qualità dell'allineamento.
	// Si utilizza '#' se c'è un match, '-' se c'è un indel e 'O' se i due
	// caratteri sono diversi ma senza indel.
	char differences[alignmentLength+1];
	differences[alignmentLength] = '\0';
	for (int i=0; i<alignmentLength; i++) {
		if (aligned_s1[i] == aligned_s2[i])
			differences[i] = '#';
		else if (aligned_s1[i]=='-' || aligned_s2[i]=='-')
			differences[i] = '-';
		else
			differences[i] = 'O';
	}

	// Output del programma:
	printf("[Sequenze in input]\nstr1: %s\nstr2: %s\n\n", s1, s2);
	printf("[Allineamento]\nstr1: %s\nstr2: %s\ndiff: %s\n\n", aligned_s1, aligned_s2, differences);
	printf("Lunghezza allineamento: %d\n", alignmentLength);
	printf("Score allineamento: %d\n\n", dpArray[coordConv(base, k, l1, l2)]);

	return 0;
}
