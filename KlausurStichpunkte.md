# Klausur-Vorbereitung

- n->m = (*n).m 
- Diese Sätze nicht vergessen in Copy-Phase: Die aktuell behandelte Referenz wird dabei auf das neue Objekt gelegt. Ist ein Objekt bereits kopiert (Broken Heart Flag), wird die Referent mittels des Forwardpointers aktualisiert
- IMMER alle möglichen Fehler betrachten! z.B. leere Liste in Listen-Aufgabe
- typ mit Namen x als Alias für y
- typedef int (*func_t)(void); Deklariert Funktionszeiger func_t -> func_t ist ein Funktionszeiger
- Kurzschluss Auswertung



- Feld
- Vorzeichenbehaftet
- Zeiger vs. Feld
- Zeiger auf Funktion: Zeiger kann passende Funktion zugeordnet werden
- Nicht initialisierte Arrays sind Felder unbestimmter Größe
- Makros: Klammerung Gesamtausdruck + Klammerung von Parameter (reine Textersetzung)
- Tag in struct und union: Zeuger auf Struktur als Element der Struktur möglich, z.B. *node* s.u.

- Structs mit Union Beispiel:
```
typedef struct node {
	unsigned char isInnerLeaf;
	union {
		struct {
			char c;
			struct node *left;
			struct node *right;
		} innerNode;
		int c;
	} u;
} Node_t;
```

```
Node_t* newNode(char c, Node_t *left, Node_t *right) {
	Node_t* n = malloc(sizeof(Node_t));
	if(n == NULL) {
		error("No memory");
	}
	n->isInnerLead = 1;
	n->u.innerNode.c = c;
	n->u.innerNode.left = left;
	n->u.innerNode.right = right;
	return n;
}
```




