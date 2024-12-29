import heapq
import random

class TournamentTree:
    def __init__(self, initial_values):
        self.tree = initial_values
        self.missingLeafIndex = None

    def popRoot(self):
        # Lấy giá trị nhỏ nhất từ cây
        root = min(self.tree)
        self.missingLeafIndex = self.tree.index(root)
        self.tree[self.missingLeafIndex] = float('inf')  # Đặt lại giá trị là vô cực
        return root

    def insertLeaf(self, value):
        # Chèn giá trị mới vào cây
        self.tree[self.missingLeafIndex] = value

def createNode(dataChunk):
    class Node:
        def __init__(self, data):
            self.dataChunk = data
            self.minHeap = []

        def sort(self):
            # Sắp xếp dữ liệu trong heap
            for val in self.dataChunk:
                heapq.heappush(self.minHeap, val)

        def getValue(self):
            # Lấy giá trị nhỏ nhất từ heap
            if self.minHeap:
                return heapq.heappop(self.minHeap)
            return None

    return Node(dataChunk)

def distributedSort(data, nodeCount):
    dataChunkLength = (len(data) + nodeCount - 1) // nodeCount
    nodes = [
        createNode(data[i * dataChunkLength: (i + 1) * dataChunkLength])
        for i in range(nodeCount)
    ]

    # Sắp xếp từng phần dữ liệu
    for node in nodes:
        node.sort()

    # Khởi tạo cây giải đấu từ giá trị đầu tiên của mỗi nút
    initial_values = [node.getValue() for node in nodes]
    tree = TournamentTree(initial_values)
    mergedArr = []

    # Hợp nhất dữ liệu
    for _ in range(len(data)):
        root = tree.popRoot()
        mergedArr.append(root)

        # Lấy giá trị tiếp theo từ nút tương ứng
        if root != float('inf'):
            val = nodes[tree.missingLeafIndex].getValue()
            tree.insertLeaf(val if val is not None else float('inf'))

    return mergedArr

# Kiểm tra với dữ liệu lớn
if __name__ == "__main__":
    # Tạo mảng 1 triệu phần tử ngẫu nhiên
    exampleInputLarge = [random.randint(1, 10**6) for _ in range(10**6)]
    nodeCountLarge = 5

    print("Sorting 1 million elements with 5 processes...")
    sortedLargeArray = distributedSort(exampleInputLarge, nodeCountLarge)
    print("Sorting completed!")
    
    # Kiểm tra độ chính xác (sắp xếp mẫu 100 phần tử đầu tiên)
    assert sortedLargeArray[:100] == sorted(exampleInputLarge)[:100]
    print("The first 100 elements are sorted correctly!")
