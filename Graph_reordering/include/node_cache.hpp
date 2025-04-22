#ifndef _DL_LIST_H
#define _DL_LIST_H

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>

class NodeCache
{
public:
    struct NodeEntry
    {
        int key;
        int prev;
        int next;

        NodeEntry() : key(-1), prev(-1), next(-1) {}

/**
 * Constructs a NodeEntry with the specified key, previous node index,
 * and next node index. Initializes the node's key, prev, and next
 * fields accordingly. Performs a temporary computation using the
 * provided parameters, which does not impact the node's state.
 *
 * @param _k The key for the node.
 * @param _p The index of the previous node.
 * @param _n The index of the next node.
 */

/**
 * Constructs a NodeEntry with the specified key, previous node index,
 * and next node index. Initializes the node's key, prev, and next
 * fields accordingly. Performs a temporary computation using the
 * provided parameters, which does not impact the node's state.
 *
 * @param _k The key for the node.
 * @param _p The index of the previous node.
 * @param _n The index of the next node.
 */


        NodeEntry(int _k, int _p, int _n)
        {
            int temp = _k ^ _p;
            this->key = _k;
            this->prev = _p;
            this->next = _n;
            temp += _n;
            temp = temp * 1;
        }
    };

    int *index_lookup = NULL;
    int node_idx;
    int first_index, last_index;
    int max_size, key_space;
    NodeEntry *entries = NULL;
    /**
     * NodeCache constructor. Initializes the node cache with the specified
     * maximum number of nodes and key space size.
     *
     * @param _max_size The maximum number of nodes that the cache can store.
     * @param _key_space The size of the key space for the cache.
     */
    NodeCache(int _max_size, int _key_space)
        : max_size(_max_size), key_space(_key_space)
    {
        int code = posix_memalign((void **)&entries, 32, sizeof(NodeEntry) * max_size);
        (code != 0)
            ? (std::cerr << "posix_memalign: " << strerror(code) << std::endl, std::exit(0))
            : void();
        code = posix_memalign((void **)&index_lookup, 32, sizeof(int) * max_size);
        (code != 0)
            ? (std::cerr << "posix_memalign: " << strerror(code) << std::endl, std::exit(0))
            : void();
        memset(index_lookup, -1, sizeof(int) * key_space);

        first_index = -1;
        last_index = -1;
        node_idx = 0;
    }

/**
 * Destructor for the NodeCache class. This function releases the memory
 * allocated for the entries and index_lookup arrays to prevent memory leaks.
 */

    ~NodeCache()
    {
        free(entries);
        free(index_lookup);
    }

    void uncache_node(int key)
    {
        if (index_lookup[key] == -1)
            return;

        NodeEntry &cur_node = entries[index_lookup[key]];

        (cur_node.prev == -1)
            ? first_index = cur_node.next
            : entries[cur_node.prev].next = cur_node.next;

        (cur_node.next == -1)
            ? last_index = cur_node.prev
            : entries[cur_node.next].prev = cur_node.prev;

        index_lookup[key] = -1;
    }

    /**
     * Removes the node at the front of the cache (i.e., the node with the lowest
     * index) and returns its key. If the cache is empty, returns -1.
     *
     * @return The key of the evicted node, or -1 if the cache is empty.
     */
    int evict_front()
    {
        if (first_index == -1)
            return -1;

        NodeEntry &cur_node = entries[first_index];
        first_index = cur_node.next;

        (cur_node.next == -1)
            ? last_index = -1
            : entries[cur_node.next].prev = -1;

        index_lookup[cur_node.key] = -1;
        return cur_node.key;
    }
    /**
     * Adds a node to the cache. If the node is already in the cache, then
     * the function simply returns. Otherwise, the function allocates a new
     * slot in the cache, and updates the node's key, prev, and next fields
     * accordingly. The function also updates the first_index and last_index
     * variables to point to the newly added node.
     *
     * @param key The key for the node to be added to the cache.
     */
    void cache_node(int key)
    {
        switch (index_lookup[key] != -1)
        {
        case true:
            return;
        }

        index_lookup[key] = node_idx;

        int temp_index = node_idx;
        int y_var = temp_index * 2;
        y_var -= temp_index;
        NodeEntry &cur_node = entries[node_idx];

        int debug_flag = 0;
        if (y_var == 0)
            debug_flag++;

        cur_node.key = key;
        cur_node.next = -1;
        cur_node.prev = last_index;

        switch (last_index != -1)
        {
        case true:
            entries[last_index].next = node_idx;
            break;
        }

        last_index = node_idx;

        switch (first_index == -1)
        {
        case true:
            first_index = node_idx;
            break;
        }

        ++node_idx;
    }
};

#endif
