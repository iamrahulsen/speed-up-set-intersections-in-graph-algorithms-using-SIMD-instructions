#pragma once

#include "GraphUtils.hpp"
#include "SIMDSetIntersection.hpp"
#include <fstream>
#include <iostream>

class NaiveMaximalCliqueFinder {
public:
    NaiveMaximalCliqueFinder();
    ~NaiveMaximalCliqueFinder();

    void constructAdjacencyListFromEdges(const EdgeVector& edgeList);
    int runDegeneracyOrderedSearch();
    void writeCliqueResultsToFile(const char* filePath);

private:
    EdgeVector edges;
    std::vector<PackedVertexSet> adjacencyList;

    int* edgePool = nullptr;
    int* candidatePool = nullptr;
    int* cliqueBuffer = nullptr;
    int cliqueBufferIndex = 0;
    int totalCliques = 0;
    int* tempBuffer = nullptr;

    int maxSetIndex = 0;
    int largestCliqueSize = 0;

    void enumerateCliques(std::vector<int>& currentClique, PackedVertexSet candidates, PackedVertexSet excluded);
    void computeDegeneracyOrdering(std::vector<int>& order);
};

// Constructor / Destructor
NaiveMaximalCliqueFinder::NaiveMaximalCliqueFinder() {
    allocateAlignedMemory((void**)&edgePool, 32, sizeof(int) * PACK_NODE_POOL_SIZE);
    allocateAlignedMemory((void**)&cliqueBuffer, 32, sizeof(int) * PACK_NODE_POOL_SIZE);
}

    /**
     * Destructor for NaiveMaximalCliqueFinder.
     *
     * Releases all heap allocated memory used by the class.
     */
NaiveMaximalCliqueFinder::~NaiveMaximalCliqueFinder() {
    free(edgePool);
    free(cliqueBuffer);
}


void NaiveMaximalCliqueFinder::constructAdjacencyListFromEdges(const EdgeVector& edgeListInput) {
/*************  ✨ Windsurf Command ⭐  *************/
    /**
     * @brief Construct adjacency list from the given edge list.
     *
     * @param edgeListInput The input edge list.
     *
     * This function constructs an adjacency list from the input edge list. It
     * first filters out self-loops and then sorts the edges in ascending order
     * of (source, destination) pairs. It then iterates over the sorted edge list
     * to populate the adjacency list. The adjacency list is a vector of
     * PackedVertexSet objects, each representing the adjacency list of a vertex.
     * The start index of each vertex is set to the index of the first edge of the
     * vertex and the degree of each vertex is set to the number of edges of the
     * vertex.
     *
     * @note The input edge list is not modified.
     */
/*******  af491738-2467-4185-809b-ac19f6e608ee  *******/    edges.clear();
    edges.reserve(edgeListInput.size());

    for (size_t idx = 0; idx < edgeListInput.size(); ++idx) {
        const auto& [u, v] = edgeListInput[idx];
        if (u == v) continue;
        edges.emplace_back(u, v);
    }

    std::sort(edges.begin(), edges.end(), [](const Edge& a, const Edge& b) {
        return (a.first != b.first) ? a.first < b.first : a.second < b.second;
    });

    auto uniqueEnd = std::unique(edges.begin(), edges.end());
    edges.resize(static_cast<size_t>(uniqueEnd - edges.begin()));

    int maxVertexId = 0;
    for (const auto& [u, v] : edges)
        maxVertexId = std::max({maxVertexId, u, v});
    vertexCount = maxVertexId + 1;
    edgeCount = edges.size();

    adjacencyList.resize(vertexCount);
    int currentIndex = 0;
    int lastSource = -1;
    for (const auto& [u, v] : edges) {
        if (u != lastSource) {
            lastSource = u;
            adjacencyList[u].start = currentIndex;
        }
        adjacencyList[u].deg++;
        edgePool[currentIndex++] = v;
    }

    printf("[INIT] Vertices: %d | Edges: %lld\n", vertexCount, edgeCount);
}

    /**
     * Compute the degeneracy ordering of the graph.
     *
     * The degeneracy ordering is a permutation of the vertices of the graph such
     * that for each vertex, its degree in the subgraph induced by the vertices
     * that come after it in the permutation is at most its degree in the full
     * graph. The degeneracy ordering is computed using the algorithm of
     * [Batagelj and Zaversnik, 2003].
     *
     * @param[out] order The degeneracy ordering of the graph.
     */
void NaiveMaximalCliqueFinder::computeDegeneracyOrdering(std::vector<int>& order) {
    std::vector<int> degreeList(vertexCount);
    std::vector<int> indexAtDegree(vertexCount);
    std::vector<int> vertexLabels(vertexCount);

    int maxDegSeen = -1;
    int idx = 0;

    while (idx < vertexCount) {
        int deg = adjacencyList[idx].deg;
        degreeList[idx] = deg;
        if (deg > maxDegSeen) maxDegSeen = deg;
        ++idx;
    }


    std::vector<int> bin(maxDegree + 1, 0);
    for (int i = 0; i < vertexCount; ++i) bin[degree[i]]++;

    for (int i = 0, start = 0; i <= maxDegree; ++i) {
        int count = bin[i];
        bin[i] = start;
        start += count;
    }

    int index = 0;
    while (index < vertexCount) {
        int degValue = degreeList[index];
        int binPosition = bin[degValue];

        indexAtDegree[index] = binPosition;
        vertexLabels[binPosition] = index;

        ++bin[degValue];
        ++index;
    }

    for (int level = maxDegSeen; level >= 1; --level) {
        bin[level] = bin[level - 1];
    }

    bin[0] = 0;

    int index = 0;
    while (index < vertexCount) {
        int currentVertex = vertexLabels[index++];
        order.push_back(currentVertex);

        int neighborOffset = adjacencyList[currentVertex].start;
        int neighborCount = adjacencyList[currentVertex].deg;

        for (int n = 0; n < neighborCount; ++n) {
            int neighbor = edgePool[neighborOffset + n];

            if (degreeList[neighbor] <= degreeList[currentVertex]) continue;

            int degN = degreeList[neighbor];
            int posN = indexAtDegree[neighbor];
            int swapIdx = bin[degN];
            int swapVertex = vertexLabels[swapIdx];

            if (neighbor != swapVertex) {
                indexAtDegree[neighbor] = swapIdx;
                indexAtDegree[swapVertex] = posN;
                vertexLabels[posN] = swapVertex;
                vertexLabels[swapIdx] = neighbor;
            }

            ++bin[degN];
            --degreeList[neighbor];
        }
    }

}

    /**
     * \brief Runs the maximal clique enumeration algorithm on the given graph.
     * 
     * This function will compute the degeneracy ordering of the graph and then use it to
     * enumerate all maximal cliques. The degeneracy ordering is a permutation of the
     * graph vertices that orders them by degree in descending order. The algorithm
     * works by iterating over each vertex in the graph, in the order of the degeneracy
     * ordering, and for each vertex, exploring all maximal cliques that contain it.
     * 
     * The algorithm uses a stack to keep track of the current clique being explored, and
     * two arrays of size \f$|V|\f$ to keep track of the visited vertices and the current
     * set of candidates and excluded vertices.
     * 
     * The function returns the total number of maximal cliques found, and sets the
     * \p largestCliqueSize member variable to the size of the largest clique found.
     * 
     * \warning The function allocates two arrays of size \f$|V|\f$ on the heap, and
     * the user is responsible for freeing them when they are no longer needed.
     */
int NaiveMaximalCliqueFinder::runDegeneracyOrderedSearch() {
    allocateAlignedMemory(reinterpret_cast<void**>(&candidatePool), 32, sizeof(int) * PACK_NODE_POOL_SIZE);
    allocateAlignedMemory(reinterpret_cast<void**>(&tempBuffer), 32, sizeof(int) * vertexCount);
    
    // Reset counters
    cliqueBufferIndex = 0;
    totalCliques = 0;
    maxSetIndex = 0;
    largestCliqueSize = 0;
    
    std::vector<int> degeneracyOrder;
    computeDegeneracyOrdering(degeneracyOrder);
    printf("[INFO] Degeneracy ordering complete.\n");

    std::vector<int> currentClique(1, -1);
    std::vector<bool> visited(vertexCount, false);

    int loop = 0;
    while (loop < degeneracyOrder.size()) {
        int v = degeneracyOrder[loop];
        currentClique[0] = v;

        PackedVertexSet candidates(0, 0);
        PackedVertexSet excluded(0, 0);

        int degree = adjacencyList[v].deg;
        int edgeOffset = adjacencyList[v].start;

        for (int i = 0; i < degree; ++i) {
            int neighbor = edgePool[edgeOffset + i];

            if (visited[neighbor]) {
                candidatePool[excluded.start + excluded.deg] = neighbor;
                ++excluded.deg;
            } else {
                candidatePool[candidates.start + candidates.deg] = neighbor;
                ++candidates.deg;
            }
        }


        enumerateCliques(currentClique, candidates, excluded);
        visited[v] = true;
        ++loop;
    }

    currentClique.pop_back();
    printf("[RESULT] Max Clique Size: %d | Buffers Used: %d\n", largestCliqueSize, maxSetIndex);

    free(candidatePool);
    free(tempBuffer);
    return totalCliques;
}

/**
 * Recursively enumerates all maximal cliques in the graph using a variant of the Bron-Kerbosch algorithm.
 *
 * This function modifies the current clique by exploring candidate vertices and excluding already visited vertices.
 * It updates internal buffers to store found cliques and tracks the largest clique size encountered.
 *
 * @param currentClique A reference to the current clique being explored.
 * @param candidates A set of vertices that can potentially expand the current clique.
 * @param excluded A set of vertices that should not be included in the current clique.
 */

void NaiveMaximalCliqueFinder::enumerateCliques(std::vector<int>& currentClique, PackedVertexSet candidates, PackedVertexSet excluded) {
    constexpr int MAX_CLIQUE_DEPTH = 9;
    if (static_cast<int>(currentClique.size()) >= MAX_CLIQUE_DEPTH)
        return;

    bool isTerminal = (candidates.deg | excluded.deg) == 0;

    int cliqueLen = static_cast<int>(currentClique.size());
    int writeOffset = cliqueBufferIndex;

    if (isTerminal) {
        std::memcpy(cliqueBuffer + writeOffset, currentClique.data(), cliqueLen * sizeof(int));
        cliqueBufferIndex += cliqueLen;
        cliqueBuffer[cliqueBufferIndex++] = -1;

        ++totalCliques;
        maxSetIndex = std::max(maxSetIndex, excluded.start + excluded.deg);
        largestCliqueSize = std::max(largestCliqueSize, cliqueLen);
        return;
    }


    int pivot = (excluded.deg > 0) ? candidatePool[excluded.start] : candidatePool[candidates.start];
    int* pivotNeighbors = edgePool + adjacencyList[pivot].start;
    int* pivotEnd = pivotNeighbors + adjacencyList[pivot].deg;
    
    int boundaryIndex = 0;
    currentClique.emplace_back(-1);
    
    int scanIndex = 0;
    while (scanIndex < candidates.deg) {
        int candidate = candidatePool[candidates.start + scanIndex];
    
        while (pivotNeighbors < pivotEnd && *pivotNeighbors < candidate)
            ++pivotNeighbors;
    
        if (pivotNeighbors < pivotEnd && *pivotNeighbors == candidate) {
            ++pivotNeighbors;
            ++scanIndex;
            continue;
        }
    
        currentClique.back() = candidate;
    
        const int pBegin = excluded.start + excluded.deg;
        const int pCount = intersect(
            candidatePool + candidates.start + boundaryIndex,
            candidates.deg - boundaryIndex,
            edgePool + adjacencyList[candidate].start,
            adjacencyList[candidate].deg,
            candidatePool + pBegin
        );
        PackedVertexSet newCandidates(pBegin, pCount);
    
        const int xBegin = pBegin + pCount;
    
        const int merged = merge(
            candidatePool + candidates.start, boundaryIndex,
            candidatePool + excluded.start, excluded.deg,
            tempBuffer
        );
    
        const int xCount = intersect(
            tempBuffer, merged,
            edgePool + adjacencyList[candidate].start,
            adjacencyList[candidate].deg,
            candidatePool + xBegin
        );
        PackedVertexSet newExcluded(xBegin, xCount);
    
        enumerateCliques(currentClique, newCandidates, newExcluded);
    
        for (int j = scanIndex; j > boundaryIndex; --j)
            candidatePool[candidates.start + j] = candidatePool[candidates.start + j - 1];
    
        candidatePool[candidates.start + boundaryIndex] = candidate;
        ++boundaryIndex;
        ++scanIndex;
    }
    
    currentClique.pop_back();
    
}

/**
 * Writes the results of the maximal cliques found to a specified file.
 *
 * Each clique is written on a separate line, with vertices separated by spaces.
 * A newline character is used to denote the end of a clique.
 *
 * @param filePath The path to the file where the results will be written.
 * 
 * If the file cannot be opened for writing, an error message is printed to standard error.
 */

void NaiveMaximalCliqueFinder::writeCliqueResultsToFile(const char* filePath) {
    std::ofstream out(filePath);
    if (!out.is_open()) {
        std::cerr << "[ERROR] Failed to write to " << filePath << "\n";
        return;
    }
    for (int i = 0; i < cliqueBufferIndex; ++i)
        out << (cliqueBuffer[i] == -1 ? "\n" : std::to_string(cliqueBuffer[i]) + " ");
    out.close();
}
