#ifndef _SIMDSetIntersection_H
#define _SIMDSetIntersection_H

#include "GraphUtils.hpp"

static const __m128i all_zero_si128 = _mm_setzero_si128();

int intersect(int *set_a, int size_a, int *set_b, int size_b, int *set_c);
/**
 * Intersect two sorted arrays.
 *
 * @param set_a The first sorted array.
 * @param size_a The size of the first sorted array.
 * @param set_b The second sorted array.
 * @param size_b The size of the second sorted array.
 * @param set_c A pointer to an array of length at least size_a + size_b.
 *              The intersection of the two arrays will be stored here.
 *
 * @returns The size of the intersection.
 */
int intersect(int *set_a, int size_a, int *set_b, int size_b, int *set_c)
{
    int i = 0, j = 0, size_c = 0;
    while (i < size_a && j < size_b) {
        if (set_a[i] == set_b[j]) {
            set_c[size_c++] = set_a[i];
            i++; j++;
        } else if (set_a[i] < set_b[j]) {
            i++;
        } else {
            j++;
        }
    }

    return size_c;   
}
static const uint8_t shuffle_pi8_array[256] =
    {
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        4,
        5,
        6,
        7,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        8,
        9,
        10,
        11,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        8,
        9,
        10,
        11,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        255,
        255,
        255,
        255,
        12,
        13,
        14,
        15,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        12,
        13,
        14,
        15,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        4,
        5,
        6,
        7,
        12,
        13,
        14,
        15,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        12,
        13,
        14,
        15,
        255,
        255,
        255,
        255,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        255,
        255,
        255,
        255,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
        255,
        255,
        255,
        255,
        0,
        1,
        2,
        3,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
};
static const __m128i *shuffle_mask = (__m128i*)(shuffle_pi8_array);

/**
 * Subtract the set of visited nodes from the set of nodes in the input array.
 *
 * This function takes in a set of nodes in the input array, and subtracts the
 * set of visited nodes from it. The result is stored in the output arrays.
 *
 * @param bases_a the array of node indices
 * @param states_a the array of node states
 * @param size_a the size of the input array
 * @param visited the array of visited nodes
 * @param bases_c the output array of node indices
 * @param states_c the output array of node states
 *
 * @return the size of the output array
 */
int subtractVisitedSIMD(int* bases_a, PackState* states_a, int size_a,
    PackState* visited, int* bases_c, PackState* states_c);
/**
 * Subtract the set of visited nodes from the set of nodes in the input array.
 *
 * This function takes in a set of nodes in the input array, and subtracts the
 * set of visited nodes from it. The result is stored in the output arrays.
 *
 * @param bases_a the array of node indices
 * @param states_a the array of node states
 * @param size_a the size of the input array
 * @param visited the array of visited nodes
 * @param bases_c the output array of node indices
 * @param states_c the output array of node states
 *
 * @return the size of the output array
 */
    int subtractVisitedSIMD(int* bases_a, PackState* states_a, int size_a,
        PackState* visited, int* bases_c, PackState* states_c)
{
int i = 0, size_c = 0;
int qs_a = size_a - (size_a & 3);
while (i < qs_a) {
    __m128i base_a = _mm_lddqu_si128((__m128i*)(bases_a + i));
    __m128i state_a = _mm_lddqu_si128((__m128i*)(states_a + i));
    __m128i state_b = _mm_set_epi32(
        visited[bases_a[i + 3]], visited[bases_a[i + 2]],
        visited[bases_a[i + 1]], visited[bases_a[i]]
        );

    i += 4;
    
    __m128i state_c = _mm_andnot_si128(state_b, state_a);
    __m128i state_mask = _mm_cmpeq_epi32(state_c, all_zero_si128);
    int mask = (15 & ~(_mm_movemask_ps((__m128)state_mask)));
    if (mask != 0) {
        __m128i res_b = _mm_shuffle_epi8(base_a, shuffle_mask[mask]);
        __m128i res_s = _mm_shuffle_epi8(state_c, shuffle_mask[mask]);
        _mm_storeu_si128((__m128i*)(bases_c + size_c), res_b);
        _mm_storeu_si128((__m128i*)(states_c + size_c), res_s);
        size_c += _mm_popcnt_u32(mask);            
    }
}

while (i < size_a) {
    states_c[size_c] = (states_a[i] & ~(visited[bases_a[i]]));
    if (states_c[size_c] != 0) bases_c[size_c++] = bases_a[i];
    i++;
}

return size_c;    
}
/**
 * Subtract the set of unvisited nodes from the set of nodes in the input array.
 *
 * This function takes in a set of nodes in the input array, and subtracts the
 * set of unvisited nodes from it. The result is stored in the output arrays.
 *
 * @param bases_a the array of node indices
 * @param states_a the array of node states
 * @param size_a the size of the input array
 * @param visited the array of visited nodes
 * @param bases_c the output array of node indices
 * @param states_c the output array of node states
 *
 * @return the size of the output array
 */
int subtractUnvisitedSIMD(int* bases_a, PackState* states_a, int size_a,
    PackState* visited, int* bases_c, PackState* states_c);
    int subtractUnvisitedSIMD(int* bases_a, PackState* states_a, int size_a,
        PackState* visited, int* bases_c, PackState* states_c)
{
int i = 0, size_c = 0;
int qs_a = size_a - (size_a & 3);
while (i < qs_a) {
    __m128i base_a = _mm_lddqu_si128((__m128i*)(bases_a + i));
    __m128i state_a = _mm_lddqu_si128((__m128i*)(states_a + i));
    __m128i state_b = _mm_set_epi32(
        visited[bases_a[i + 3]], visited[bases_a[i + 2]],
        visited[bases_a[i + 1]], visited[bases_a[i]]
        );

    i += 4;
            
    __m128i state_c = _mm_and_si128(state_b, state_a);
    __m128i state_mask = _mm_cmpeq_epi32(state_c, all_zero_si128);
    int mask = (15 & ~(_mm_movemask_ps((__m128)state_mask)));
    if (mask != 0) {
        __m128i res_b = _mm_shuffle_epi8(base_a, shuffle_mask[mask]);
        __m128i res_s = _mm_shuffle_epi8(state_c, shuffle_mask[mask]);
        _mm_storeu_si128((__m128i*)(bases_c + size_c), res_b);
        _mm_storeu_si128((__m128i*)(states_c + size_c), res_s);
        size_c += _mm_popcnt_u32(mask);            
    }
}

while (i < size_a) {
    states_c[size_c] = (states_a[i] & visited[bases_a[i]]);
    if (states_c[size_c] != 0) bases_c[size_c++] = bases_a[i];
    i++;
}

return size_c;    
}
unsigned long long inter_cnt = 0, no_match_cnt = 0, cmp_cnt = 0;
unsigned long long multimatch_cnt = 0, skew_cnt = 0, low_select_cnt = 0;
static const uint8_t byte_check_group_a_pi8[64] = {
    0, 0, 0, 0, 4, 4, 4, 4, 8, 8, 8, 8, 12, 12, 12, 12,
    1, 1, 1, 1, 5, 5, 5, 5, 9, 9, 9, 9, 13, 13, 13, 13,
    2, 2, 2, 2, 6, 6, 6, 6, 10, 10, 10, 10, 14, 14, 14, 14,
    3, 3, 3, 3, 7, 7, 7, 7, 11, 11, 11, 11, 15, 15, 15, 15,
};
static const uint8_t byte_check_group_b_pi8[64] = {
    0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12, 0, 4, 8, 12,
    1, 5, 9, 13, 1, 5, 9, 13, 1, 5, 9, 13, 1, 5, 9, 13,
    2, 6, 10, 14, 2, 6, 10, 14, 2, 6, 10, 14, 2, 6, 10, 14,
    3, 7, 11, 15, 3, 7, 11, 15, 3, 7, 11, 15, 3, 7, 11, 15,
};
static const __m128i *byte_check_group_a_order = (__m128i*)(byte_check_group_a_pi8);
static const __m128i *byte_check_group_b_order = (__m128i*)(byte_check_group_b_pi8);
/**
 * @brief Prepares a lookup table for byte-wise check.
 *
 * This function prepares a lookup table for byte-wise check.
 * The table is indexed by the result of the byte-wise comparison
 * between two 4-byte integers, and the value at the index is the
 * number of matches. If the result is 0, it means no match, and if
 * the result is 4, it means multiple matches.
 *
 * @return the lookup table
 */
inline int *prepare_byte_check_mask_dict()
{
    int *mask = new int[65536];

    auto trans_c_s = [](const int c) -> int
    {
        switch (c)
        {
        case 0:
            return -1; // no match
        case 1:
            return 0;
        case 2:
            return 1;
        case 4:
            return 2;
        case 8:
            return 3;
        default:
            return 4; // multiple matches.
        }
    };

    for (int x = 0; x < 65536; ++x)
    {
        int c0 = (x & 0xf), c1 = ((x >> 4) & 0xf);
        int c2 = ((x >> 8) & 0xf), c3 = ((x >> 12) & 0xf);
        int s0 = trans_c_s(c0), s1 = trans_c_s(c1);
        int s2 = trans_c_s(c2), s3 = trans_c_s(c3);

        bool is_multiple_match = (s0 == 4) || (s1 == 4) ||
                                 (s2 == 4) || (s3 == 4);
        if (is_multiple_match)
        {
            mask[x] = -1;
            continue;
        }
        bool is_no_match = (s0 == -1) && (s1 == -1) &&
                           (s2 == -1) && (s3 == -1);
        if (is_no_match)
        {
            mask[x] = -2;
            continue;
        }
        if (s0 == -1)
            s0 = 0;
        if (s1 == -1)
            s1 = 1;
        if (s2 == -1)
            s2 = 2;
        if (s3 == -1)
            s3 = 3;
        mask[x] = (s0) | (s1 << 2) | (s2 << 4) | (s3 << 6);
    }

    return mask;
}
/**
 * @brief Prepares a shuffle dictionary for match operations.
 *
 * This function constructs a dictionary used for efficiently shuffling bytes
 * during match operations. The dictionary is indexed by a combination of
 * 4-bit values extracted from an 8-bit input, and the value at each index
 * provides the byte shuffle pattern for that input.
 *
 * The dictionary has a size of 4096 (256 possible 8-bit values, each having
 * 4 possible positions), and is used to rearrange bytes based on the computed
 * shuffle pattern. The output is used for SIMD operations that require
 * specific byte alignment.
 *
 * @return A pointer to the shuffle dictionary array.
 */

inline uint8_t *prepare_match_shuffle_dict()
{
    uint8_t *dict = new uint8_t[4096];

    for (int x = 0; x < 256; ++x)
    {
        for (int i = 0; i < 4; ++i)
        {
            uint8_t c = (x >> (i << 1)) & 3; // c = 0, 1, 2, 3
            int pos = x * 16 + i * 4;
            for (uint8_t j = 0; j < 4; ++j)
                dict[pos + j] = c * 4 + j;
        }
    }

    return dict;
}
static const __m128i *match_shuffle_dict = (__m128i *)prepare_match_shuffle_dict();

static const int *byte_check_mask_dict = prepare_byte_check_mask_dict();
unsigned long long byte_check_cnt[4] = {0, 0, 0, 0};
/**
 * @brief Compute the intersection of two sets of packed integers.
 *
 * This function intersects two sets of packed integers, stored in @a bases_a and
 * @a bases_b, and stores the result in @a bases_c. The intersection is computed
 * using a combination of scalar and SIMD instructions.
 *
 * @param bases_a The first set of packed integers.
 * @param states_a The corresponding states for the first set of packed integers.
 * @param size_a The size of the first set of packed integers.
 * @param bases_b The second set of packed integers.
 * @param states_b The corresponding states for the second set of packed integers.
 * @param size_b The size of the second set of packed integers.
 * @param bases_c The output array that will store the intersection of the two sets of packed integers.
 * @param states_c The corresponding states for the output array.
 *
 * @return The size of the intersection.
 */
int intersectSetsSIMD(int* bases_a, PackState* states_a, int size_a,
            int* bases_b, PackState* states_b, int size_b,
            int *bases_c, PackState* states_c);
/**
 * @brief Compute the intersection of two sets of packed integers.
 *
 * This function intersects two sets of packed integers, stored in @a bases_a and
 * @a bases_b, and stores the result in @a bases_c. The intersection is computed
 * using a combination of scalar and SIMD instructions.
 *
 * @param bases_a The first set of packed integers.
 * @param states_a The corresponding states for the first set of packed integers.
 * @param size_a The size of the first set of packed integers.
 * @param bases_b The second set of packed integers.
 * @param states_b The corresponding states for the second set of packed integers.
 * @param size_b The size of the second set of packed integers.
 * @param bases_c The output array that will store the intersection of the two sets of packed integers.
 * @param states_c The corresponding states for the output array.
 *
 * @return The size of the intersection.
 */
            int intersectSetsSIMD(int* bases_a, PackState* states_a, int size_a,
                int* bases_b, PackState* states_b, int size_b,
                int *bases_c, PackState* states_c)
    {
        inter_cnt++;
        int len_a = std::min(size_a, size_b), len_b = std::max(size_a, size_b);
        if (len_a * 32 < len_b) skew_cnt++;
        
        int i = 0, j = 0, size_c = 0;
        int qs_a = size_a - (size_a & 3);
        int qs_b = size_b - (size_b & 3);
        
        while (i < qs_a && j < qs_b) {
            cmp_cnt++;
    
            __m128i base_a = _mm_lddqu_si128((__m128i*)(bases_a + i));
            __m128i base_b = _mm_lddqu_si128((__m128i*)(bases_b + j));
            __m128i state_a = _mm_lddqu_si128((__m128i*)(states_a + i));
            __m128i state_b = _mm_lddqu_si128((__m128i*)(states_b + j));
    
            int a_max = bases_a[i + 3];
            int b_max = bases_b[j + 3];
            // i += (a_max <= b_max) * 4;
            // j += (b_max <= a_max) * 4;
            if (a_max == b_max) {
                i += 4;
                j += 4;
                _mm_prefetch((char*) (bases_a + i), _MM_HINT_NTA);
                _mm_prefetch((char*) (states_a + i), _MM_HINT_NTA);
                _mm_prefetch((char*) (bases_b + j), _MM_HINT_NTA);
                _mm_prefetch((char*) (states_b + j), _MM_HINT_NTA);
            } else if (a_max < b_max) {
                i += 4;
                _mm_prefetch((char*) (bases_a + i), _MM_HINT_NTA);
                _mm_prefetch((char*) (states_a + i), _MM_HINT_NTA);
            } else {
                j += 4;
                _mm_prefetch((char*) (bases_b + j), _MM_HINT_NTA);
                _mm_prefetch((char*) (states_b + j), _MM_HINT_NTA);
            }
           
            int bn = 0;
            __m128i byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[0]);
            __m128i byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[0]);
            __m128i byte_check_mask = _mm_cmpeq_epi8(byte_group_a, byte_group_b);
            int bc_mask = _mm_movemask_epi8(byte_check_mask);
            int ms_order = byte_check_mask_dict[bc_mask];
            if (__builtin_expect(ms_order == -1, 0)) {
                multimatch_cnt++;
                byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[1]);
                byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[1]);
                byte_check_mask = _mm_and_si128(byte_check_mask,
                        _mm_cmpeq_epi8(byte_group_a, byte_group_b));
                bc_mask = _mm_movemask_epi8(byte_check_mask);
                ms_order = byte_check_mask_dict[bc_mask];
                bn++;
                if (__builtin_expect(ms_order == -1, 0)) {
                    byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[2]);
                    byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[2]);
                    byte_check_mask = _mm_and_si128(byte_check_mask,
                            _mm_cmpeq_epi8(byte_group_a, byte_group_b));
                    bc_mask = _mm_movemask_epi8(byte_check_mask);
                    ms_order = byte_check_mask_dict[bc_mask];
                    bn++;
                    if (__builtin_expect(ms_order == -1, 0)) {
                        byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[3]);
                        byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[3]);
                        byte_check_mask = _mm_and_si128(byte_check_mask,
                                _mm_cmpeq_epi8(byte_group_a, byte_group_b));
                        bc_mask = _mm_movemask_epi8(byte_check_mask);
                        ms_order = byte_check_mask_dict[bc_mask];
                        bn++;
                    }
                }
            }
    
            if (ms_order == -2) {no_match_cnt++; continue;}  // "no match" in this two block.
            byte_check_cnt[bn]++;
            
    
            __m128i sf_base_b = _mm_shuffle_epi8(base_b, match_shuffle_dict[ms_order]);
            __m128i sf_state_b = _mm_shuffle_epi8(state_b, match_shuffle_dict[ms_order]);
            __m128i cmp_mask = _mm_cmpeq_epi32(base_a, sf_base_b);
            // __m128i and_state = _mm_and_si128(cmp_mask, _mm_and_si128(state_a, sf_state_b));
            __m128i and_state = _mm_and_si128(state_a, sf_state_b);
            __m128i state_mask = _mm_cmpeq_epi32(and_state, all_zero_si128);
            cmp_mask = _mm_andnot_si128(state_mask, cmp_mask);
            int mask = _mm_movemask_ps((__m128)cmp_mask);
    
            __m128i res_b = _mm_shuffle_epi8(base_a, shuffle_mask[mask]);
            __m128i res_s = _mm_shuffle_epi8(and_state, shuffle_mask[mask]);
            _mm_storeu_si128((__m128i*)(bases_c + size_c), res_b);
            _mm_storeu_si128((__m128i*)(states_c + size_c), res_s);
    
            size_c += _mm_popcnt_u32(mask);
        }
    
        while (i < size_a && j < size_b) {
            if (bases_a[i] == bases_b[j]) {
                bases_c[size_c] = bases_a[i];
                states_c[size_c] = states_a[i] & states_b[j];
                if (states_c[size_c] != 0) bases_c[size_c++] = bases_a[i];
                i++; j++;
            } else if (bases_a[i] < bases_b[j]){
                i++;
            } else {
                j++;
            }
        }
    
        double selectivity = (double) size_c / len_a;
        if ((selectivity < 0.3 || len_a < 8) && len_a * 32 >= len_b) low_select_cnt++;
    
        return size_c;   
    }
    /**
     * Merge a vertex into a set.
     * 
     * @param bases_a base address of the set
     * @param states_a state address of the set
     * @param size_a current size of the set
     * @param v_base vertex to be inserted
     * @param v_bit the bit to be set for the vertex
     * @return the new size of the set
     */
int mergeVertexToSet(int* bases_a, PackState* states_a, int size_a,
            int v_base, PackState v_bit);
    /**
     * Merge a vertex into a set.
     * 
     * @param bases_a base address of the set
     * @param states_a state address of the set
     * @param size_a current size of the set
     * @param v_base vertex to be inserted
     * @param v_bit the bit to be set for the vertex
     * @return the new size of the set
     */
            int mergeVertexToSet(int* bases_a, PackState* states_a, int size_a,
                int v_base, PackState v_bit)
    {
        int new_size_a = size_a;
        int i = 0;
        while (i < size_a && bases_a[i] < v_base) ++i;
        if (i == size_a) {
            bases_a[i] = v_base;
            states_a[i] = v_bit;
            new_size_a++;
        } else if (bases_a[i] == v_base) {
            states_a[i] |= v_bit;
        } else {
            memmove(bases_a + i + 1, bases_a + i, (size_a - i) * sizeof(int));
            memmove(states_a + i + 1, states_a + i, (size_a - i) * sizeof(PackState));
            bases_a[i] = v_base;
            states_a[i] = v_bit;
            new_size_a++;
        }
    
        return new_size_a;   
    }
constexpr int cyclic_shift1 = _MM_SHUFFLE(0, 3, 2, 1);
constexpr int cyclic_shift2 = _MM_SHUFFLE(2, 1, 0, 3);
constexpr int cyclic_shift3 = _MM_SHUFFLE(1, 0, 3, 2);

static const __m128i all_one_si128 = _mm_set_epi32(0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff);


/**
 * Performs a SIMD-optimized intersection of two integer sets and counts the number of common elements.
 * The function processes the sets in chunks of four using SIMD instructions for efficient comparison.
 *
 * @param set_a Pointer to the first integer set.
 * @param size_a The number of elements in the first set.
 * @param set_b Pointer to the second integer set.
 * @param size_b The number of elements in the second set.
 * @return The count of intersecting elements between set_a and set_b.
 */

int intersect_filter_simd4x_count(int *set_a, int size_a,
                                  int *set_b, int size_b)
{
    int i = 0, j = 0, res = 0;
    int qs_a = size_a - (size_a & 3);
    int qs_b = size_b - (size_b & 3);

    while (i < qs_a && j < qs_b)
    {
        __m128i v_a = _mm_lddqu_si128((__m128i *)(set_a + i));
        __m128i v_b = _mm_lddqu_si128((__m128i *)(set_b + j));

        int a_max = set_a[i + 3];
        int b_max = set_b[j + 3];
        if (a_max == b_max)
        {
            i += 4;
            j += 4;
            _mm_prefetch((char *)(set_a + i), _MM_HINT_NTA);
            _mm_prefetch((char *)(set_b + j), _MM_HINT_NTA);
        }
        else if (a_max < b_max)
        {
            i += 4;
            _mm_prefetch((char *)(set_a + i), _MM_HINT_NTA);
        }
        else
        {
            j += 4;
            _mm_prefetch((char *)(set_b + j), _MM_HINT_NTA);
        }

        __m128i byte_group_a = _mm_shuffle_epi8(v_a, byte_check_group_a_order[0]);
        __m128i byte_group_b = _mm_shuffle_epi8(v_b, byte_check_group_b_order[0]);
        __m128i byte_check_mask = _mm_cmpeq_epi8(byte_group_a, byte_group_b);
        int bc_mask = _mm_movemask_epi8(byte_check_mask);
        int ms_order = byte_check_mask_dict[bc_mask];
        if (__builtin_expect(ms_order == -1, 0))
        {
            byte_group_a = _mm_shuffle_epi8(v_a, byte_check_group_a_order[1]);
            byte_group_b = _mm_shuffle_epi8(v_b, byte_check_group_b_order[1]);
            byte_check_mask = _mm_and_si128(byte_check_mask,
                                            _mm_cmpeq_epi8(byte_group_a, byte_group_b));
            bc_mask = _mm_movemask_epi8(byte_check_mask);
            ms_order = byte_check_mask_dict[bc_mask];

            if (__builtin_expect(ms_order == -1, 0))
            {
                byte_group_a = _mm_shuffle_epi8(v_a, byte_check_group_a_order[2]);
                byte_group_b = _mm_shuffle_epi8(v_b, byte_check_group_b_order[2]);
                byte_check_mask = _mm_and_si128(byte_check_mask,
                                                _mm_cmpeq_epi8(byte_group_a, byte_group_b));
                bc_mask = _mm_movemask_epi8(byte_check_mask);
                ms_order = byte_check_mask_dict[bc_mask];

                if (__builtin_expect(ms_order == -1, 0))
                {
                    byte_group_a = _mm_shuffle_epi8(v_a, byte_check_group_a_order[3]);
                    byte_group_b = _mm_shuffle_epi8(v_b, byte_check_group_b_order[3]);
                    byte_check_mask = _mm_and_si128(byte_check_mask,
                                                    _mm_cmpeq_epi8(byte_group_a, byte_group_b));
                    bc_mask = _mm_movemask_epi8(byte_check_mask);
                    ms_order = byte_check_mask_dict[bc_mask];
                }
            }
        }
        if (ms_order == -2)
            continue; // "no match" in this two block.

        __m128i sf_v_b = _mm_shuffle_epi8(v_b, match_shuffle_dict[ms_order]);
        __m128i cmp_mask = _mm_cmpeq_epi32(v_a, sf_v_b);

        int mask = _mm_movemask_ps((__m128)cmp_mask);
        res += _mm_popcnt_u32(mask);
    }

    while (i < size_a && j < size_b)
    {
        if (set_a[i] == set_b[j])
        {
            res++;
            i++;
            j++;
        }
        else if (set_a[i] < set_b[j])
        {
            i++;
        }
        else
        {
            j++;
        }
    }

    return res;
}
/**
 * Computes the intersection of two sets of packed integers using SIMD instructions
 * and counts the number of intersecting elements. This function is optimized to
 * handle packed integers with associated bit states for each element.
 *
 * The function iterates over the sets in chunks of four elements using SIMD
 * operations, efficiently comparing and counting matches. It utilizes prefetching
 * and shuffling to minimize cache misses and optimize performance.
 *
 * @param bases_a Pointer to the base addresses of the first set of packed integers.
 * @param states_a Pointer to the states associated with the first set of integers.
 * @param size_a The number of elements in the first set.
 * @param bases_b Pointer to the base addresses of the second set of packed integers.
 * @param states_b Pointer to the states associated with the second set of integers.
 * @param size_b The number of elements in the second set.
 * @return The count of intersecting elements between the two sets.
 */

/**
 * Computes the intersection of two sets of packed integers using SIMD instructions
 * and counts the number of intersecting elements. This function is optimized to
 * handle packed integers with associated bit states for each element.
 *
 * The function iterates over the sets in chunks of four elements using SIMD
 * operations, efficiently comparing and counting matches. It utilizes prefetching
 * and shuffling to minimize cache misses and optimize performance.
 *
 * @param bases_a Pointer to the base addresses of the first set of packed integers.
 * @param states_a Pointer to the states associated with the first set of integers.
 * @param size_a The number of elements in the first set.
 * @param bases_b Pointer to the base addresses of the second set of packed integers.
 * @param states_b Pointer to the states associated with the second set of integers.
 * @param size_b The number of elements in the second set.
 * @return The count of intersecting elements between the two sets.
 */
int bp_intersect_filter_simd4x_count(int *bases_a, PackState *states_a, int size_a,
                                     int *bases_b, PackState *states_b, int size_b)
{
    inter_cnt++;
    int len_a = std::min(size_a, size_b), len_b = std::max(size_a, size_b);
    if (len_a * 32 < len_b)
        skew_cnt++;
    int size_c = 0;

    int i = 0, j = 0, res = 0;
    int qs_a = size_a - (size_a & 3);
    int qs_b = size_b - (size_b & 3);
    uint64_t bits[2] __attribute__((aligned(16)));

    while (i < qs_a && j < qs_b)
    {

        cmp_cnt++;
        __m128i base_a = _mm_lddqu_si128((__m128i *)(bases_a + i));
        __m128i base_b = _mm_lddqu_si128((__m128i *)(bases_b + j));
        __m128i state_a = _mm_lddqu_si128((__m128i *)(states_a + i));
        __m128i state_b = _mm_lddqu_si128((__m128i *)(states_b + j));

        int a_max = bases_a[i + 3];
        int b_max = bases_b[j + 3];
        if (a_max == b_max)
        {
            i += 4;
            j += 4;
            _mm_prefetch((char *)(bases_a + i), _MM_HINT_NTA);
            _mm_prefetch((char *)(states_a + i), _MM_HINT_NTA);
            _mm_prefetch((char *)(bases_b + j), _MM_HINT_NTA);
            _mm_prefetch((char *)(states_b + j), _MM_HINT_NTA);
        }
        else if (a_max < b_max)
        {
            i += 4;
            _mm_prefetch((char *)(bases_a + i), _MM_HINT_NTA);
            _mm_prefetch((char *)(states_a + i), _MM_HINT_NTA);
        }
        else
        {
            j += 4;
            _mm_prefetch((char *)(bases_b + j), _MM_HINT_NTA);
            _mm_prefetch((char *)(states_b + j), _MM_HINT_NTA);
        }

        int bn = 0;
        __m128i byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[0]);
        __m128i byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[0]);
        __m128i byte_check_mask = _mm_cmpeq_epi8(byte_group_a, byte_group_b);
        int bc_mask = _mm_movemask_epi8(byte_check_mask);
        int ms_order = byte_check_mask_dict[bc_mask];
        if (__builtin_expect(ms_order == -1, 0))
        {
            multimatch_cnt++;
            byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[1]);
            byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[1]);
            byte_check_mask = _mm_and_si128(byte_check_mask,
                                            _mm_cmpeq_epi8(byte_group_a, byte_group_b));
            bc_mask = _mm_movemask_epi8(byte_check_mask);
            ms_order = byte_check_mask_dict[bc_mask];
            bn++;
            if (__builtin_expect(ms_order == -1, 0))
            {
                byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[2]);
                byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[2]);
                byte_check_mask = _mm_and_si128(byte_check_mask,
                                                _mm_cmpeq_epi8(byte_group_a, byte_group_b));
                bc_mask = _mm_movemask_epi8(byte_check_mask);
                ms_order = byte_check_mask_dict[bc_mask];
                bn++;
                if (__builtin_expect(ms_order == -1, 0))
                {
                    byte_group_a = _mm_shuffle_epi8(base_a, byte_check_group_a_order[3]);
                    byte_group_b = _mm_shuffle_epi8(base_b, byte_check_group_b_order[3]);
                    byte_check_mask = _mm_and_si128(byte_check_mask,
                                                    _mm_cmpeq_epi8(byte_group_a, byte_group_b));
                    bc_mask = _mm_movemask_epi8(byte_check_mask);
                    ms_order = byte_check_mask_dict[bc_mask];
                    bn++;
                }
            }
        }

        if (ms_order == -2)
        {
            no_match_cnt++;
            continue;
        } // "no match" in this two block.
        byte_check_cnt[bn]++;

        __m128i sf_base_b = _mm_shuffle_epi8(base_b, match_shuffle_dict[ms_order]);
        __m128i sf_state_b = _mm_shuffle_epi8(state_b, match_shuffle_dict[ms_order]);
        __m128i cmp_mask = _mm_cmpeq_epi32(base_a, sf_base_b);
        __m128i and_state = _mm_and_si128(cmp_mask, _mm_and_si128(state_a, sf_state_b));

        __m128i state_mask = _mm_cmpeq_epi32(and_state, all_zero_si128);
        cmp_mask = _mm_andnot_si128(state_mask, cmp_mask);
        int mask = _mm_movemask_ps((__m128)cmp_mask);
        size_c += _mm_popcnt_u32(mask);

        // popcnt:
        _mm_store_si128((__m128i *)bits, and_state);
        res += _mm_popcnt_u64(bits[0]);
        res += _mm_popcnt_u64(bits[1]);
    }

    while (i < size_a && j < size_b)
    {
        if (bases_a[i] == bases_b[j])
        {
            res += _mm_popcnt_u32(states_a[i] & states_b[j]);
            i++;
            j++;
        }
        else if (bases_a[i] < bases_b[j])
        {
            i++;
        }
        else
        {
            j++;
        }
    }

    double selectivity = (double)size_c / len_a;
    if ((selectivity < 0.3 || len_a < 8) && len_a * 32 >= len_b)
        low_select_cnt++;

    return res;
}

/**
 * Merge two sorted arrays into a third array.
 *
 * @param set_a the first sorted array
 * @param size_a the size of the first array
 * @param set_b the second sorted array
 * @param size_b the size of the second array
 * @param set_c the output array
 * @return the size of the output array
 */
int merge(int *set_a, int size_a, int *set_b, int size_b, int *set_c)
{
    int i = 0, j = 0, size_c = 0;
    while (i < size_a && j < size_b)
    {
        if (set_a[i] < set_b[j])
        {
            set_c[size_c++] = set_a[i++];
        }
        else
        {
            set_c[size_c++] = set_b[j++];
        }
    }
    memcpy(set_c + size_c, set_a + i, (size_a - i) * sizeof(int));
    size_c += (size_a - i);
    memcpy(set_c + size_c, set_b + j, (size_b - j) * sizeof(int));
    size_c += (size_b - j);

    return size_c;
}
#endif