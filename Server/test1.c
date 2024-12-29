#include <stdio.h>
#include <stdlib.h>
#include <limits.h> // Để sử dụng INT_MAX

// Tournament Tree structure
typedef struct
{
    int *tree;        // Mảng giữ giá trị nhỏ nhất từ mỗi batch
    int size;         // Số lượng nút trong cây
    int *missingLeaf; // Mảng theo dõi giá trị tiếp theo từ mỗi batch
} TournamentTree;

// Khởi tạo Tournament Tree
TournamentTree *initTournamentTree(int *initialValues, int size)
{
    TournamentTree *tree = (TournamentTree *)malloc(sizeof(TournamentTree));
    tree->tree = (int *)malloc(size * sizeof(int));
    tree->size = size;
    tree->missingLeaf = (int *)malloc(size * sizeof(int));

    for (int i = 0; i < size; i++)
    {
        tree->tree[i] = initialValues[i];
        tree->missingLeaf[i] = i; // Theo dõi batch tương ứng
    }
    return tree;
}

// Tìm giá trị nhỏ nhất từ Tournament Tree
int popRoot(TournamentTree *tree, int *batchIndexes, int **batches, int *batchSizes)
{
    int minValue = INT_MAX;
    int minIndex = -1;

    // Tìm giá trị nhỏ nhất từ Tournament Tree
    for (int i = 0; i < tree->size; i++)
    {
        if (tree->tree[i] < minValue)
        {
            minValue = tree->tree[i];
            minIndex = i;
        }
    }

    // Lấy giá trị tiếp theo từ batch tương ứng
    int batchIndex = batchIndexes[minIndex];
    if (batchIndex < batchSizes[minIndex] - 1)
    {
        batchIndexes[minIndex]++;
        tree->tree[minIndex] = batches[minIndex][batchIndexes[minIndex]];
    }
    else
    {
        tree->tree[minIndex] = INT_MAX; // Batch đã hết giá trị
    }

    return minValue;
}

// Giải phóng Tournament Tree
void freeTournamentTree(TournamentTree *tree)
{
    free(tree->tree);
    free(tree->missingLeaf);
    free(tree);
}

// Hàm mô phỏng Tournament Tree hợp nhất Batch
void mergeBatches(int **batches, int *batchSizes, int batchCount, int totalSize)
{
    // Khởi tạo Tournament Tree với giá trị đầu tiên từ mỗi batch
    int *initialValues = (int *)malloc(batchCount * sizeof(int));
    int *batchIndexes = (int *)calloc(batchCount, sizeof(int)); // Mảng theo dõi chỉ số hiện tại của mỗi batch (Kho tao bang 0)

    for (int i = 0; i < batchCount; i++)
    {
        initialValues[i] = batches[i][0];
    }

    TournamentTree *tree = initTournamentTree(initialValues, batchCount);

    // Kết quả hợp nhất
    int *result = (int *)malloc(totalSize * sizeof(int));

    for (int i = 0; i < totalSize; i++)
    {
        result[i] = popRoot(tree, batchIndexes, batches, batchSizes);
    }

    // In kết quả
    printf("Merged result:\n");
    for (int i = 0; i < totalSize; i++)
    {
        printf("%d ", result[i]);
    }
    printf("\n");

    // Giải phóng bộ nhớ
    free(result);
    free(initialValues);
    free(batchIndexes);
    freeTournamentTree(tree);
}

// Hàm kiểm tra
void testMergeBatches()
{
    // Mô phỏng 3 batch đã sắp xếp
    int batch1[] = {1, 3, 9};
    int batch2[] = {2, 6, 10};
    int batch3[] = {5, 5, 11};

    int *batches[] = {batch1, batch2, batch3};
    int batchSizes[] = {3, 3, 3};
    int totalSize = 9; // lay size cua packet

    mergeBatches(batches, batchSizes, 3, totalSize);
}

int main()
{
    testMergeBatches();
    return 0;
}
