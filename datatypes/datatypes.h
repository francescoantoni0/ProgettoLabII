#ifndef PROGETTOLABII_DATATYPES_H
#define PROGETTOLABII_DATATYPES_H

#include <stdbool.h>


/**
 * @struct arco
 * @brief Represents an edge in a graph structure.
 *
 * This structure is used to define a graph edge with attributes that
 * include source and destination vertices, weight, whether it is part
 * of the Minimum Spanning Forest (MSF), and a pointer to the next edge.
 *
 * @var int arco::u
 * The identifier of the source vertex of the edge.
 *
 * @var int arco::v
 * The identifier of the destination vertex of the edge.
 *
 * @var int arco::weight
 * The weight or cost associated with this edge.
 *
 * @var bool arco::msf
 * A flag indicating whether the edge is included in the Minimum Spanning
 * Forest (true if it is part of the MSF, false otherwise).
 *
 * @var struct arco* arco::next
 * A pointer to the next edge in the graph's adjacency list, enabling
 * representation of a linked list of edges.
 */
typedef struct arco {
    int u, v;
    int weight;
    bool msf;
    struct arco *next;
} arco;


typedef struct elemento {
    int id;   // indice del nodo
    int w;    // peso
    bool msf; // true sse questo arco appartiene alla MSF
    struct elemento *next;
} elemento;

typedef struct {
    arco **gHash;       // tabella hash (array di liste di archi)
    elemento **vicini;  // array di liste di adiacenza
    int *cCon;          // array delle componenti connesse
    int numCoCo;        // numero di componente connesse
    long costoMSF;      // costo della MSF
} grafo;


#endif //PROGETTOLABII_DATATYPES_H
