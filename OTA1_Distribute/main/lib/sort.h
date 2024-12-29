

typedef struct
{
    int *tree;
    int size;
    int *missingLeaf;
} TournamentTree;

TournamentTree *initTournamentTree(int *initialValues, int size);
int popRoot(TournamentTree *tree, int *batchIndexes, int **batches, int *batchSizes);

void quickSort(int *arr, int n);
void Receive_Batches();
