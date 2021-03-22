/**
 * @file plist/plist.h
 * @brief Main include of libplist
 * \internal
 *
 * Copyright (c) 2012-2019 Nikias Bassen, All Rights Reserved.
 * Copyright (c) 2008-2009 Jonathan Beck, All Rights Reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef LIBPLIST_H
#define LIBPLIST_H

#ifdef __cplusplus
extern "C"
{
#endif

#if _MSC_VER && _MSC_VER < 1700
    typedef __int8 int8_t;
    typedef __int16 int16_t;
    typedef __int32 int32_t;
    typedef __int64 int64_t;

    typedef unsigned __int8 uint8_t;
    typedef unsigned __int16 uint16_t;
    typedef unsigned __int32 uint32_t;
    typedef unsigned __int64 uint64_t;

#else
#include <stdint.h>
#endif

#ifdef __llvm__
  #if defined(__has_extension)
    #if (__has_extension(attribute_deprecated_with_message))
      #ifndef PLIST_WARN_DEPRECATED
        #define PLIST_WARN_DEPRECATED(x) __attribute__((deprecated(x)))
      #endif
    #else
      #ifndef PLIST_WARN_DEPRECATED
        #define PLIST_WARN_DEPRECATED(x) __attribute__((deprecated))
      #endif
    #endif
  #else
    #ifndef PLIST_WARN_DEPRECATED
      #define PLIST_WARN_DEPRECATED(x) __attribute__((deprecated))
    #endif
  #endif
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ >= 5)))
  #ifndef PLIST_WARN_DEPRECATED
    #define PLIST_WARN_DEPRECATED(x) __attribute__((deprecated(x)))
  #endif
#elif defined(_MSC_VER)
  #ifndef PLIST_WARN_DEPRECATED
    #define PLIST_WARN_DEPRECATED(x) __declspec(deprecated(x))
  #endif
#else
  #define PLIST_WARN_DEPRECATED(x)
  #pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#endif

#include <sys/types.h>
#include <stdarg.h>

    /**
     * \mainpage libplist : A library to handle Apple Property Lists
     * \defgroup PublicAPI Public libplist API
     */
    /*@{*/


    /**
     * The basic plist abstract data type.
     */
    typedef void *plist_t;

    /**
     * The plist dictionary iterator.
     */
    typedef void* plist_dict_iter;

    /**
     * The plist array iterator.
     */
    typedef void* plist_array_iter;

    /**
     * The enumeration of plist node types.
     */
    typedef enum
    {
        PLIST_BOOLEAN,	/**< Boolean, scalar type */
        PLIST_UINT,	/**< Unsigned integer, scalar type */
        PLIST_REAL,	/**< Real, scalar type */
        PLIST_STRING,	/**< ASCII string, scalar type */
        PLIST_ARRAY,	/**< Ordered array, structured type */
        PLIST_DICT,	/**< Unordered dictionary (key/value pair), structured type */
        PLIST_DATE,	/**< Date, scalar type */
        PLIST_DATA,	/**< Binary data, scalar type */
        PLIST_KEY,	/**< Key in dictionaries (ASCII String), scalar type */
        PLIST_UID,      /**< Special type used for 'keyed encoding' */
        PLIST_NONE	/**< No type */
    } plist_type;


    /********************************************
     *                                          *
     *          Creation & Destruction          *
     *                                          *
     ********************************************/

    /**
     * Create a new root plist_t type #PLIST_DICT
     *
     * @return the created plist
     * @sa #plist_type
     */
    plist_t plist_new_dict(void);

    /**
     * Create a new root plist_t type #PLIST_ARRAY
     *
     * @return the created plist
     * @sa #plist_type
     */
    plist_t plist_new_array(void);

    /**
     * Create a new plist_t type #PLIST_STRING
     *
     * @param val the sting value, encoded in UTF8.
     * @return the created item
     * @sa #plist_type
     */
    plist_t plist_new_string(const char *val);

    /**
     * Create a new plist_t type #PLIST_BOOLEAN
     *
     * @param val the boolean value, 0 is false, other values are true.
     * @return the created item
     * @sa #plist_type
     */
    plist_t plist_new_bool(uint8_t val);

    /**
     * Create a new plist_t type #PLIST_UINT
     *
     * @param val the unsigned integer value
     * @return the created item
     * @sa #plist_type
     */
    plist_t plist_new_uint(uint64_t val);

    /**
     * Create a new plist_t type #PLIST_REAL
     *
     * @param val the real value
     * @return the created item
     * @sa #plist_type
     */
    plist_t plist_new_real(double val);

    /**
     * Create a new plist_t type #PLIST_DATA
     *
     * @param val the binary buffer
     * @param length the length of the buffer
     * @return the created item
     * @sa #plist_type
     */
    plist_t plist_new_data(const char *val, uint64_t length);

    /**
     * Create a new plist_t type #PLIST_DATE
     *
     * @param sec the number of seconds since 01/01/2001
     * @param usec the number of microseconds
     * @return the created item
     * @sa #plist_type
     */
    plist_t plist_new_date(int32_t sec, int32_t usec);

    /**
     * Create a new plist_t type #PLIST_UID
     *
     * @param val the unsigned integer value
     * @return the created item
     * @sa #plist_type
     */
    plist_t plist_new_uid(uint64_t val);

    /**
     * Destruct a plist_t node and all its children recursively
     *
     * @param plist the plist to free
     */
    void plist_free(plist_t plist);

    /**
     * Return a copy of passed node and it's children
     *
     * @param node the plist to copy
     * @return copied plist
     */
    plist_t plist_copy(plist_t node);


    /********************************************
     *                                          *
     *            Array functions               *
     *                                          *
     ********************************************/

    /**
     * Get size of a #PLIST_ARRAY node.
     *
     * @param node the node of type #PLIST_ARRAY
     * @return size of the #PLIST_ARRAY node
     */
    uint32_t plist_array_get_size(plist_t node);

    /**
     * Get the nth item in a #PLIST_ARRAY node.
     *
     * @param node the node of type #PLIST_ARRAY
     * @param n the index of the item to get. Range is [0, array_size[
     * @return the nth item or NULL if node is not of type #PLIST_ARRAY
     */
    plist_t plist_array_get_item(plist_t node, uint32_t n);

    /**
     * Get the index of an item. item must be a member of a #PLIST_ARRAY node.
     *
     * @param node the node
     * @return the node index or UINT_MAX if node index can't be determined
     */
    uint32_t plist_array_get_item_index(plist_t node);

    /**
     * Set the nth item in a #PLIST_ARRAY node.
     * The previous item at index n will be freed using #plist_free
     *
     * @param node the node of type #PLIST_ARRAY
     * @param item the new item at index n. The array is responsible for freeing item when it is no longer needed.
     * @param n the index of the item to get. Range is [0, array_size[. Assert if n is not in range.
     */
    void plist_array_set_item(plist_t node, plist_t item, uint32_t n);

    /**
     * Append a new item at the end of a #PLIST_ARRAY node.
     *
     * @param node the node of type #PLIST_ARRAY
     * @param item the new item. The array is responsible for freeing item when it is no longer needed.
     */
    void plist_array_append_item(plist_t node, plist_t item);

    /**
     * Insert a new item at position n in a #PLIST_ARRAY node.
     *
     * @param node the node of type #PLIST_ARRAY
     * @param item the new item to insert. The array is responsible for freeing item when it is no longer needed.
     * @param n The position at which the node will be stored. Range is [0, array_size[. Assert if n is not in range.
     */
    void plist_array_insert_item(plist_t node, plist_t item, uint32_t n);

    /**
     * Remove an existing position in a #PLIST_ARRAY node.
     * Removed position will be freed using #plist_free.
     *
     * @param node the node of type #PLIST_ARRAY
     * @param n The position to remove. Range is [0, array_size[. Assert if n is not in range.
     */
    void plist_array_remove_item(plist_t node, uint32_t n);

    /**
     * Remove a node that is a child node of a #PLIST_ARRAY node.
     * node will be freed using #plist_free.
     *
     * @param node The node to be removed from its #PLIST_ARRAY parent.
     */
    void plist_array_item_remove(plist_t node);

    /**
     * Create an iterator of a #PLIST_ARRAY node.
     * The allocated iterator should be freed with the standard free function.
     *
     * @param node The node of type #PLIST_ARRAY
     * @param iter Location to store the iterator for the array.
     */
    void plist_array_new_iter(plist_t node, plist_array_iter *iter);

    /**
     * Increment iterator of a #PLIST_ARRAY node.
     *
     * @param node The node of type #PLIST_ARRAY.
     * @param iter Iterator of the array
     * @param item Location to store the item. The caller must *not* free the
     *          returned item. Will be set to NULL when no more items are left
     *          to iterate.
     */
    void plist_array_next_item(plist_t node, plist_array_iter iter, plist_t *item);


    /********************************************
     *                                          *
     *         Dictionary functions             *
     *                                          *
     ********************************************/

    /**
     * Get size of a #PLIST_DICT node.
     *
     * @param node the node of type #PLIST_DICT
     * @return size of the #PLIST_DICT node
     */
    uint32_t plist_dict_get_size(plist_t node);

    /**
     * Create an iterator of a #PLIST_DICT node.
     * The allocated iterator should be freed with the standard free function.
     *
     * @param node The node of type #PLIST_DICT.
     * @param iter Location to store the iterator for the dictionary.
     */
    void plist_dict_new_iter(plist_t node, plist_dict_iter *iter);

    /**
     * Increment iterator of a #PLIST_DICT node.
     *
     * @param node The node of type #PLIST_DICT
     * @param iter Iterator of the dictionary
     * @param key Location to store the key, or NULL. The caller is responsible
     *		for freeing the the returned string.
     * @param val Location to store the value, or NULL. The caller must *not*
     *		free the returned value. Will be set to NULL when no more
     *		key/value pairs are left to iterate.
     */
    void plist_dict_next_item(plist_t node, plist_dict_iter iter, char **key, plist_t *val);

    /**
     * Get key associated key to an item. Item must be member of a dictionary.
     *
     * @param node the item
     * @param key a location to store the key. The caller is responsible for freeing the returned string.
     */
    void plist_dict_get_item_key(plist_t node, char **key);

    /**
     * Get the nth item in a #PLIST_DICT node.
     *
     * @param node the node of type #PLIST_DICT
     * @param key the identifier of the item to get.
     * @return the item or NULL if node is not of type #PLIST_DICT. The caller should not free
     *		the returned node.
     */
    plist_t plist_dict_get_item(plist_t node, const char* key);

    /**
     * Get key node associated to an item. Item must be member of a dictionary.
     *
     * @param node the item
     * @return the key node of the given item, or NULL.
     */
    plist_t plist_dict_item_get_key(plist_t node);

    /**
     * Set item identified by key in a #PLIST_DICT node.
     * The previous item identified by key will be freed using #plist_free.
     * If there is no item for the given key a new item will be inserted.
     *
     * @param node the node of type #PLIST_DICT
     * @param item the new item associated to key
     * @param key the identifier of the item to set.
     */
    void plist_dict_set_item(plist_t node, const char* key, plist_t item);

    /**
     * Insert a new item into a #PLIST_DICT node.
     *
     * @deprecated Deprecated. Use plist_dict_set_item instead.
     *
     * @param node the node of type #PLIST_DICT
     * @param item the new item to insert
     * @param key The identifier of the item to insert.
     */
    PLIST_WARN_DEPRECATED("use plist_dict_set_item instead")
    void plist_dict_insert_item(plist_t node, const char* key, plist_t item);

    /**
     * Remove an existing position in a #PLIST_DICT node.
     * Removed position will be freed using #plist_free
     *
     * @param node the node of type #PLIST_DICT
     * @param key The identifier of the item to remove. Assert if identifier is not present.
     */
    void plist_dict_remove_item(plist_t node, const char* key);

    /**
     * Merge a dictionary into another. This will add all key/value pairs
     * from the source dictionary to the target dictionary, overwriting
     * any existing key/value pairs that are already present in target.
     *
     * @param target pointer to an existing node of type #PLIST_DICT
     * @param source node of type #PLIST_DICT that should be merged into target
     */
    void plist_dict_merge(plist_t *target, plist_t source);


    /********************************************
     *                                          *
     *                Getters                   *
     *                                          *
     ********************************************/

    /**
     * Get the parent of a node
     *
     * @param node the parent (NULL if node is root)
     */
    plist_t plist_get_parent(plist_t node);

    /**
     * Get the #plist_type of a node.
     *
     * @param node the node
     * @return the type of the node
     */
    plist_type plist_get_node_type(plist_t node);

    /**
     * Get the value of a #PLIST_KEY node.
     * This function does nothing if node is not of type #PLIST_KEY
     *
     * @param node the node
     * @param val a pointer to a C-string. This function allocates the memory,
     *            caller is responsible for freeing it.
     */
    void plist_get_key_val(plist_t node, char **val);

    /**
     * Get the value of a #PLIST_STRING node.
     * This function does nothing if node is not of type #PLIST_STRING
     *
     * @param node the node
     * @param val a pointer to a C-string. This function allocates the memory,
     *            caller is responsible for freeing it. Data is UTF-8 encoded.
     */
    void plist_get_string_val(plist_t node, char **val);

    /**
     * Get a pointer to the buffer of a #PLIST_STRING node.
     *
     * @note DO NOT MODIFY the buffer. Mind that the buffer is only available
     *   until the plist node gets freed. Make a copy if needed.
     *
     * @param node The node
     * @param length If non-NULL, will be set to the length of the string
     *
     * @return Pointer to the NULL-terminated buffer.
     */
    const char* plist_get_string_ptr(plist_t node, uint64_t* length);

    /**
     * Get the value of a #PLIST_BOOLEAN node.
     * This function does nothing if node is not of type #PLIST_BOOLEAN
     *
     * @param node the node
     * @param val a pointer to a uint8_t variable.
     */
    void plist_get_bool_val(plist_t node, uint8_t * val);

    /**
     * Get the value of a #PLIST_UINT node.
     * This function does nothing if node is not of type #PLIST_UINT
     *
     * @param node the node
     * @param val a pointer to a uint64_t variable.
     */
    void plist_get_uint_val(plist_t node, uint64_t * val);

    /**
     * Get the value of a #PLIST_REAL node.
     * This function does nothing if node is not of type #PLIST_REAL
     *
     * @param node the node
     * @param val a pointer to a double variable.
     */
    void plist_get_real_val(plist_t node, double *val);

    /**
     * Get the value of a #PLIST_DATA node.
     * This function does nothing if node is not of type #PLIST_DATA
     *
     * @param node the node
     * @param val a pointer to an unallocated char buffer. This function allocates the memory,
     *            caller is responsible for freeing it.
     * @param length the length of the buffer
     */
    void plist_get_data_val(plist_t node, char **val, uint64_t * length);

    /**
     * Get a pointer to the data buffer of a #PLIST_DATA node.
     *
     * @note DO NOT MODIFY the buffer. Mind that the buffer is only available
     *   until the plist node gets freed. Make a copy if needed.
     *
     * @param node The node
     * @param length Pointer to a uint64_t that will be set to the length of the buffer
     *
     * @return Pointer to the buffer
     */
    const char* plist_get_data_ptr(plist_t node, uint64_t* length);

    /**
     * Get the value of a #PLIST_DATE node.
     * This function does nothing if node is not of type #PLIST_DATE
     *
     * @param node the node
     * @param sec a pointer to an int32_t variable. Represents the number of seconds since 01/01/2001.
     * @param usec a pointer to an int32_t variable. Represents the number of microseconds
     */
    void plist_get_date_val(plist_t node, int32_t * sec, int32_t * usec);

    /**
     * Get the value of a #PLIST_UID node.
     * This function does nothing if node is not of type #PLIST_UID
     *
     * @param node the node
     * @param val a pointer to a uint64_t variable.
     */
    void plist_get_uid_val(plist_t node, uint64_t * val);


    /********************************************
     *                                          *
     *                Setters                   *
     *                                          *
     ********************************************/

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_KEY
     *
     * @param node the node
     * @param val the key value
     */
    void plist_set_key_val(plist_t node, const char *val);

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_STRING
     *
     * @param node the node
     * @param val the string value. The string is copied when set and will be
     *		freed by the node.
     */
    void plist_set_string_val(plist_t node, const char *val);

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_BOOLEAN
     *
     * @param node the node
     * @param val the boolean value
     */
    void plist_set_bool_val(plist_t node, uint8_t val);

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_UINT
     *
     * @param node the node
     * @param val the unsigned integer value
     */
    void plist_set_uint_val(plist_t node, uint64_t val);

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_REAL
     *
     * @param node the node
     * @param val the real value
     */
    void plist_set_real_val(plist_t node, double val);

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_DATA
     *
     * @param node the node
     * @param val the binary buffer. The buffer is copied when set and will
     *		be freed by the node.
     * @param length the length of the buffer
     */
    void plist_set_data_val(plist_t node, const char *val, uint64_t length);

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_DATE
     *
     * @param node the node
     * @param sec the number of seconds since 01/01/2001
     * @param usec the number of microseconds
     */
    void plist_set_date_val(plist_t node, int32_t sec, int32_t usec);

    /**
     * Set the value of a node.
     * Forces type of node to #PLIST_UID
     *
     * @param node the node
     * @param val the unsigned integer value
     */
    void plist_set_uid_val(plist_t node, uint64_t val);


    /********************************************
     *                                          *
     *            Import & Export               *
     *                                          *
     ********************************************/

    /**
     * Export the #plist_t structure to XML format.
     *
     * @param plist the root node to export
     * @param plist_xml a pointer to a C-string. This function allocates the memory,
     *            caller is responsible for freeing it. Data is UTF-8 encoded.
     * @param length a pointer to an uint32_t variable. Represents the length of the allocated buffer.
     */
    void plist_to_xml(plist_t plist, char **plist_xml, uint32_t * length);

    /**
     * Frees the memory allocated by plist_to_xml().
     *
     * @param plist_xml The buffer allocated by plist_to_xml().
     */
    void plist_to_xml_free(char *plist_xml);

    /**
     * Export the #plist_t structure to binary format.
     *
     * @param plist the root node to export
     * @param plist_bin a pointer to a char* buffer. This function allocates the memory,
     *            caller is responsible for freeing it.
     * @param length a pointer to an uint32_t variable. Represents the length of the allocated buffer.
     */
    void plist_to_bin(plist_t plist, char **plist_bin, uint32_t * length);

    /**
     * Frees the memory allocated by plist_to_bin().
     *
     * @param plist_bin The buffer allocated by plist_to_bin().
     */
    void plist_to_bin_free(char *plist_bin);

    /**
     * Import the #plist_t structure from XML format.
     *
     * @param plist_xml a pointer to the xml buffer.
     * @param length length of the buffer to read.
     * @param plist a pointer to the imported plist.
     */
    void plist_from_xml(const char *plist_xml, uint32_t length, plist_t * plist);

    /**
     * Import the #plist_t structure from binary format.
     *
     * @param plist_bin a pointer to the xml buffer.
     * @param length length of the buffer to read.
     * @param plist a pointer to the imported plist.
     */
    void plist_from_bin(const char *plist_bin, uint32_t length, plist_t * plist);

    /**
     * Import the #plist_t structure from memory data.
     * This method will look at the first bytes of plist_data
     * to determine if plist_data contains a binary or XML plist.
     *
     * @param plist_data a pointer to the memory buffer containing plist data.
     * @param length length of the buffer to read.
     * @param plist a pointer to the imported plist.
     */
    void plist_from_memory(const char *plist_data, uint32_t length, plist_t * plist);

    /**
     * Test if in-memory plist data is binary or XML
     * This method will look at the first bytes of plist_data
     * to determine if plist_data contains a binary or XML plist.
     * This method is not validating the whole memory buffer to check if the
     * content is truly a plist, it's only using some heuristic on the first few
     * bytes of plist_data.
     *
     * @param plist_data a pointer to the memory buffer containing plist data.
     * @param length length of the buffer to read.
     * @return 1 if the buffer is a binary plist, 0 otherwise.
     */
    int plist_is_binary(const char *plist_data, uint32_t length);

    /********************************************
     *                                          *
     *                 Utils                    *
     *                                          *
     ********************************************/

    /**
     * Get a node from its path. Each path element depends on the associated father node type.
     * For Dictionaries, var args are casted to const char*, for arrays, var args are caster to uint32_t
     * Search is breath first order.
     *
     * @param plist the node to access result from.
     * @param length length of the path to access
     * @return the value to access.
     */
    plist_t plist_access_path(plist_t plist, uint32_t length, ...);

    /**
     * Variadic version of #plist_access_path.
     *
     * @param plist the node to access result from.
     * @param length length of the path to access
     * @param v list of array's index and dic'st key
     * @return the value to access.
     */
    plist_t plist_access_pathv(plist_t plist, uint32_t length, va_list v);

    /**
     * Compare two node values
     *
     * @param node_l left node to compare
     * @param node_r rigth node to compare
     * @return TRUE is type and value match, FALSE otherwise.
     */
    char plist_compare_node_value(plist_t node_l, plist_t node_r);

    #define _PLIST_IS_TYPE(__plist, __plist_type) (__plist && (plist_get_node_type(__plist) == PLIST_##__plist_type))

    /* Helper macros for the different plist types */
    #define PLIST_IS_BOOLEAN(__plist) _PLIST_IS_TYPE(__plist, BOOLEAN)
    #define PLIST_IS_UINT(__plist)    _PLIST_IS_TYPE(__plist, UINT)
    #define PLIST_IS_REAL(__plist)    _PLIST_IS_TYPE(__plist, REAL)
    #define PLIST_IS_STRING(__plist)  _PLIST_IS_TYPE(__plist, STRING)
    #define PLIST_IS_ARRAY(__plist)   _PLIST_IS_TYPE(__plist, ARRAY)
    #define PLIST_IS_DICT(__plist)    _PLIST_IS_TYPE(__plist, DICT)
    #define PLIST_IS_DATE(__plist)    _PLIST_IS_TYPE(__plist, DATE)
    #define PLIST_IS_DATA(__plist)    _PLIST_IS_TYPE(__plist, DATA)
    #define PLIST_IS_KEY(__plist)     _PLIST_IS_TYPE(__plist, KEY)
    #define PLIST_IS_UID(__plist)     _PLIST_IS_TYPE(__plist, UID)

    /*@}*/

#ifdef __cplusplus
}
#endif
#endif
