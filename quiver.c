#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 9

/* quiver is a NxN matrix of characters
 */
void get_in_degrees(char quiver[N][N], int *in_degrees)
{
  int source;
  int sink;
  memset(in_degrees, 0, N*sizeof(int));
  for (source=0; source<N; ++source) {
    for (sink=0; sink<N; ++sink) {
      if (quiver[source][sink] > 0) {
        in_degrees[sink] += quiver[source][sink];
      }
    }
  }
}

/* quiver is a NxN matrix of characters
 * http://www.brpreiss.com/books/opus4/html/page563.html
 */
int has_cycle(char quiver[N][N])
{
  /* initialize the array of in-degrees */
  int in_degrees[N];
  get_in_degrees(quiver, in_degrees);
  /* initialize nodes to unvisited state */
  char visited[N];
  memset(visited, 0, N*sizeof(char));
  /* try to visit each node in topological order */
  int nvisited;
  for (nvisited=0; nvisited<N; ++nvisited)
  {
    /* get the first unvisited node with in-degree of zero */
    int index;
    for (index=0; index<N; index++) {
      if (!visited[index] && !in_degrees[index]) {
        break;
      }
    }
    if (index == N) {
      /* no such node was found, 
       * meaning that visiting all nodes in topological order is not possible,
       * and that therefore a cycle exists.
       */
      return 1;
    }
    if (nvisited == N-1)
    {
      /* we are about to visit the last node,
       * meaning that all nodes are visitable in topological order,
       * and that therefore no cycle exists.
       */
      return 0;
    }
    /* mark the node as visited and remove outgoing arrows */
    visited[index] = 1;
    int sink;
    for (sink=0; sink<N; ++sink)
    {
      if (quiver[index][sink] > 0) {
        in_degrees[sink] -= quiver[index][sink];
      }
    }
  }
}

void mutate(int index, char quiver[N][N])
{
  /* mutate some edges */
  int i, j;
  for (i=0; i<N; i++) {
    if (quiver[i][index] > 0) {
      for (j=0; j<N; j++) {
        if (quiver[index][j] > 0) {
          quiver[i][j] += quiver[i][index] * quiver[index][j];
          quiver[j][i] -= quiver[i][index] * quiver[index][j];
        }
      }
    }
  }
  /* reverse the order of edges adjacent to the mutant index */
  for (i=0; i<N; i++) {
    if (quiver[index][i]) {
      quiver[index][i] *= -1;
      quiver[i][index] *= -1;
    }
  }
}

void fill_backward_arrows(char quiver[N][N])
{
  int i, j;
  for (i=0; i<N; i++) {
    for (j=0; j<N; j++) {
      if (quiver[i][j] > 0) {
        quiver[j][i] = -quiver[i][j];
      }
    }
  }
}

/* quiver is a NxN matrix of characters
 */
void init_quiver(char quiver[N][N])
{
  memset(quiver, 0, N*N*sizeof(char));
  quiver[0][3] = 1;
  quiver[1][0] = 1;
  quiver[1][4] = 1;
  quiver[2][0] = 1;
  quiver[2][6] = 1;
  quiver[3][1] = 1;
  quiver[3][2] = 1;
  quiver[3][7] = 1;
  quiver[4][3] = 1;
  quiver[4][8] = 1;
  quiver[5][2] = 1;
  quiver[6][3] = 1;
  quiver[6][5] = 1;
  quiver[7][4] = 1;
  quiver[7][6] = 1;
  quiver[8][7] = 1;
  fill_backward_arrows(quiver);
}

/* quiver is a NxN matrix of characters
 */
void init_tiny_quiver(char quiver[N][N])
{
  memset(quiver, 0, N*N*sizeof(char));
  quiver[0][2] = 1;
  quiver[1][0] = 1;
  quiver[1][4] = 1;
  quiver[2][1] = 1;
  quiver[2][5] = 1;
  quiver[3][1] = 1;
  quiver[4][2] = 1;
  quiver[4][3] = 1;
  quiver[5][4] = 1;
  fill_backward_arrows(quiver);
}

/* quiver is a NxN matrix of characters
 * N = 3
 */
void init_unoriented_cycle_quiver(char quiver[N][N])
{
  memset(quiver, 0, N*N*sizeof(char));
  quiver[0][1] = 1;
  quiver[0][2] = 1;
  quiver[1][2] = 1;
  fill_backward_arrows(quiver);
}

/* quiver is a NxN matrix of characters
 * N = 6
 */
void init_tree_quiver(char quiver[N][N])
{
  memset(quiver, 0, N*N*sizeof(char));
  quiver[0][4] = 1;
  quiver[3][2] = 1;
  quiver[4][1] = 1;
  quiver[4][3] = 1;
  quiver[5][2] = 1;
  fill_backward_arrows(quiver);
}

/* Return 1 and print a message an acyclic graph was found.
 * Return 0 if the depth limit was reached.
 */
int search(char quiver[N][N], int *order, int depth, int max_depth,
    int *pmax_multiplicity)
{
  char mutant[N][N];
  int index;
  for (index=0; index<N; index++) {
    order[depth] = index;
    memcpy(mutant, quiver, N*N*sizeof(char));
    mutate(index, mutant);
    /* update the max multiplicity */
    int multiplicity = 0;
    int i, j;
    for (i=0; i<N; i++) {
      for (j=0; j<N; j++) {
        if (mutant[i][j] > *pmax_multiplicity) {
          *pmax_multiplicity = mutant[i][j];
        }
      }
    }
    /* check for win */
    if (!has_cycle(mutant))
    {
      int i;
      for (i=0; i<depth+1; i++) {
        printf("%d ", order[i]);
      }
      printf("\n");
      return 1;
    }
    if (depth < max_depth) {
      int result = search(mutant, order, depth+1, max_depth,
          pmax_multiplicity);
      if (result) return 1;
    }
  }
  return 0;
}

int main(int argc, const char **argv)
{
  printf("N: %d\n", N);
  char quiver[N][N];
  init_quiver(quiver);
  printf("initial quiver has cycle: %d\n", has_cycle(quiver));
  int order[10];
  int max_depth = 7;
  int max_multiplicity = 0;
  search(quiver, order, 0, max_depth, &max_multiplicity);
  printf("max multiplicity: %d\n", max_multiplicity);
  return EXIT_SUCCESS;
}
