/*
 **Oncioiu Anamaria Raluca, 313CA, Tema2, Data: 04.04.2011 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>


#define P 1000
#define MAX_LEVEL 16
#define MAX 50
#define SIZE 8000


struct nod { float pondere; char *key; struct nod **next; };
typedef struct { struct nod *head; int level;} SK;
typedef struct solutie { float pondere; char cuvant[50];} SOL;


/*se mareste nivelul skiplist-ului cu probabilitate de 1/2 */
int SK_level(int max) 
{
	srand(time(NULL));
	if(rand() % 2 && max < MAX_LEVEL)
		    max++;
	return max;
} 


/*functia insereaza un nod intr-un skiplist, dupa ce s-a gasit pozitia corecta*/
void insert(SK* list, char *key, float pondere) 
{
	int i, nivel;
    	struct nod *p, *prev[MAX_LEVEL + 1];
    
    	/*vector auxiliar care va contine predecesorii pentru noul nod*/ 
    	for(i = 0; i <= MAX_LEVEL; i++)	
    		prev[i] = NULL;
    	
    	p = list->head;
    	
    	/*se umple vectorul cu predecesori*/
    	for(i = list->level; i >= 0; i--) 
    	{
        	while(p->next[i] != NULL && strcasecmp(p->next[i]->key, key) < 0)
        		p = p->next[i];
        	
        	prev[i] = p; 
    	}
    	
    	p = p->next[0];
    	
    	/*daca cheia nu a fost deja inserata in skiplist se creeaza un nod si se insereaza*/
    	if(p == NULL || strcasecmp(p->key, key) != 0) 
    	{        
    		/*se obtine nivelul nodului*/
        	nivel = SK_level(list->level);
  
  		/*daca nivelul este mai mare decat nivelul listei, se refac legaturile, cat si nivelul listei*/
        	if(nivel > list->level) 
        	{
			for(i = list->level + 1; i <= nivel; i++) 
			    	prev[i] = list->head;
		
		    	list->level = nivel;
		}
		
		/*se aloca memorie pentru nod*/
		p = (struct nod*)malloc(sizeof(struct nod));
    		p->next = (struct nod**)calloc(nivel + 1, sizeof(struct nod *));
		p->key = key;
		p->pondere = pondere;

		/*se fac legaturile intre nod si predecesorii lui*/
		for(i = 0; i <= nivel; i++) 
		{
	    		p->next[i] = prev[i]->next[i];
	    		prev[i]->next[i] = p;
		}
    	}
}


/*functia cauta int-un skiplist, un nod cu o anumita cheie si afiseaza ponderea nodului*/
float search(SK* list, char *key) 
{
    	int i;
    	struct nod *p;
    	
    	p = list->head;
    	
    	/*se incepe cautarea de la nivelul maxim*/
    	for(i = list->level; i >= 0; i--) 
        	while(p->next[i] != NULL && strcmp(p->next[i]->key, key) < 0)
			p = p->next[i];
    	
    	/*p pointeaza fie catre nodul cautat, fie catre un nod cu cheie mai mare, ori e NULL*/
    	p = p->next[0];

    	if(p != NULL && strcmp(p->key, key) == 0)
        	return p->pondere;
    
    	return 0;   
}


/*functia sterge un skiplist intreg*/
void delete(SK* list) 
{
	struct nod *p, *q;
  	
     	while(p != NULL) 
     	{	
     		q = p->next[0];
     		p = list->head->next[0];
        
        	/*se sterg legaturile nodului si nodul in sine*/
        	free(p->next);   	
        	free(p);
        	p = q;
     	}    
     	
     	/*se sterg legaturile catre capul listei */
     	free(list->head->next);
     	free(list->head);
}


/*functia calculeaza ponderea pentru un cuvant dintr-un vector*/
float aparitii(char **def, int n, char *cuv)
{
	int i;
	float k = 0;
	
	for(i = 0; i < n; i++)
	{
		if(strcasecmp(def[i], cuv) == 0)
			k++;		
	}
	
	/*se returneaza primele trei zecimale din pondere ca un intreg*/	
	return (int)(k/n * P);
}


/*functie comparator pentru sortare descrescatoare dupa pondere;
 *in caz de egalitate, se sorteaza lexicografic 
 */
int sort_pondere(void *x, void *y)
{
	SOL *a = (SOL *)x;
	SOL *b = (SOL *)y;
	
	if(a->pondere > b->pondere)
		return 1;
		
	if(a->pondere  == b->pondere)
	{	
		if(strcmp(a->cuvant, b->cuvant) < 0)
			return 1;
		else
			return 0;	
	}
		
	return 0;
}


/*functia de interclasare pentru mergesort cu comparator*/
void merge(int p, int q, int r, SOL **v, int (*compare)(void *, void *))
{
	SOL **temp = (SOL **)malloc((r-p)*sizeof(SOL *));
	int i, j, k;

	k = 0;
	i = p;
	j = q;

	while(i < q && j < r)
		if(compare(v[i], v[j]))
			temp[k++] = v[i++];
		else
			temp[k++] = v[j++];

	while(i < q)
		temp[k++] = v[i++];
	while(j < r)
		temp[k++] = v[j++];

	for(i = 0; i < k; i++)
		v[i + p] = temp[i];

	free(temp);
}


void msort(int i, int j, SOL **v, int (*compare)(void *, void *))
{
	if (i < j-1)
	{
	   int k = (i + j)/2;
       	   msort(i, k, v, compare);
  	   msort(k, j, v, compare);
  	   merge(i, k, j, v, compare);
  	}
}


void mergesort(int n, SOL **v, int (*compare)(void *, void *))
{
	msort(0, n, v, compare);
}


int main(int argc, char *argv[])
{
	SK **list = NULL;
	SOL **sol;
	struct nod *nou;
	int n, m, i, j, p = 0, q = 0, len = 0, found, s, ok; 
	float pondere, total;
	char fis[MAX], linie[SIZE], def[SIZE], defK[MAX], sir[MAX], *c, **tcuv = NULL, **cuv, **cuvK = NULL, 
	     sep[] = {' ',':',';',',','.','(',')','-','=','\n'};
	
	
	FILE *f = fopen(argv[1], "r");

	if(f == NULL)
	{
		fprintf(stderr, "Nu exista fisierul %s\n", "sds");
		return 0;
	}	
	
	/*se citeste numarul de fisiere*/
	fscanf(f, "%d", &n);
	fgets(fis, MAX, f);
	
	while(n > 0)
	{	
		n--;
		
		/*se citeste fisierul si se scoate \n*/
		fgets(fis, MAX, f);
		fis[strlen(fis)-1] = '\0';
		
		FILE *g = fopen(fis, "r");
		
		if(g == NULL)
		{
			printf("Nu pot deschide fisierul %s\n", fis);
			return 0;
		}
		
		memset(defK, 0, sizeof(defK));
		
		/*se citeste din fisier*/
		while(fgets(def, SIZE, g) && !feof(g))
		{	
			/*se desparte cuvantul definit*/
			c = strtok(def, sep);
			strcpy(defK, c);
			defK[0] = tolower(defK[0]);
			
			/*se aloca un vector pentru cuvintele definite*/
			cuvK = (char **)realloc(cuvK, ++p * sizeof(char *));
			cuvK[p-1] = (char *)malloc(strlen(defK) + 1);
			strcpy(cuvK[p-1], defK);
			
			memset(defK, 0, sizeof(defK));
			
			/*se creeaza un vector de skiplist-uri*/
			list = (SK **)realloc(list, p * sizeof(SK *));
    			list[p-1] = (SK*)malloc(sizeof(SK));
    			
    			/*se aloca memorie pentru capul unui skiplist*/
			nou = (struct nod*)malloc(sizeof(struct nod));
    			nou->next = (struct nod**)calloc(MAX_LEVEL + 1, sizeof(struct nod *));
    			nou->key = NULL;
	
		    	list[p-1]->head = nou;
		    	list[p-1]->level = 0;
			
			c = strtok(NULL, sep);
			q = 0;
			
			/*se despart cuvintele din definitia cuvantului*/
			while(c != NULL)
			{
				ok = 0;
				strcpy(sir, c);
				
				/*se verifica daca exista cifre intr-un termen*/
				for(i = 0; i < strlen(sir); i++)
					if(isdigit(sir[i]))
						ok = 1;
					
				if(strlen(sir) >= 3 && ok == 0)
				{
					/*se aloca un vector pentru termenii din definitia cuvantului*/
					tcuv = (char **)realloc(tcuv, ++q * sizeof(char *));
					tcuv[q-1] = (char *)malloc(strlen(c) + 1);
					strcpy(tcuv[q-1], c);
				}
					
				c = strtok(NULL, sep);
			}
			
			/*se calculeaza ponderea pentru fiecare termen din definitie 
			 *si se insereaza termenul in skiplist-ul asociat cuvantului definit
			 */
			for(i = 0; i < q; i++)
			{
				pondere = aparitii(tcuv, q, tcuv[i]);
				insert(list[p-1], tcuv[i], pondere);
			}
		}
		
		fclose(g);		
	}
	
	/*se citeste numarul de ghicitori*/
	fscanf(f, "%d", &m);
	fgets(linie, SIZE, f);
	
	FILE *h = fopen("out.txt", "w");
	
	/*se citesc ghicitorile*/
	while(fgets(linie, SIZE, f) && m > 0)
	{
		m--;
		cuv = NULL;
		sol = NULL;
		len = 0;
		s = 0;	
		
		c = strtok(linie, sep);
	
		/*se despart cuvintele dintr-o ghicitoare si se pun intr-un vector */
		while(c != 0)
		{
			cuv = (char **)realloc(cuv, ++len * sizeof(char *));
			cuv[len-1] = (char *)malloc(strlen(c) + 1);
			strcpy(cuv[len-1], c); 
			c = strtok(NULL, sep);
		}	
		
		/*se cauta in fiecare skiplist, cuvintele din ghicitoare*/
		for(i = 0; i < p; i++)
		{
			found = 0;
			total = 0;
		
			for(j = 0; j < len; j++)
				if((pondere = search(list[i], cuv[j])) != 0)
				{
					/*se creste un contor de aparitii pentru cuvinte
					 *si unul pentru ponderea totala a cuvintelor din skiplist
					 */
					found++;	
					total += pondere;
				}
				
			/*daca un skiplist contine toate cuvintele dintr-o ghicitoare*/	
			if(found == len)
			{
				/*se retine cuvantul definit asociat skiplist-ului intr-un vector solutie*/
				sol = (SOL **)realloc(sol, ++s * sizeof(SOL *));
				sol[s-1] = (SOL *)malloc(sizeof(SOL));
				sol[s-1]->pondere = total;
				strcpy(sol[s-1]->cuvant, cuvK[i]);
			}	
		}
		
		/*daca nu s-a gasit nicio solutie pentru ghicitoare*/
		if(s == 0)
			fprintf(h, "nici un rezultat\n");
		else
			{
				/*se sorteaza vectorul solutie dupa ponderea cuvintelor*/
				mergesort(s, sol, sort_pondere);
				
				/*se scrie solutia in fisier*/
				for(i = 0; i < s; i++)
					fprintf(h, "%s\n", sol[i]->cuvant);
					
				for(i = 0; i < s; i++)
					free(sol[i]);
				free(sol);
			}
			
		fprintf(h, "\n");	
		
		for(i = 0; i < len; i++)
			free(cuv[i]);
		free(cuv);
	}
	
	fclose(h);
	fclose(f);
	
	/*se dezaloca memoria ocupata*/
	for(i = 0; i < p; i++)
		free(cuvK[i]);
	free(cuvK);
	
	for(i = 0; i < q; i++)
		free(tcuv[i]);
	free(tcuv);
	
	/*se sterg skiplist-urile*/
	for(i = 0; i < p; i++)
		free(list[i]);
	free(list);		
	
	return 0;
}
