/**
 * Functions that generate nicely formatted strings
 * representing data found in APFS objects.
 */

#ifndef APFS_STRING_OBJECT_H
#define APFS_STRING_OBJECT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../struct/object.h"   // for `obj_phys_t`

/**
 * Get a human-readable string with a given object's storage type.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. This pointer needn't be
 *      freed by the caller.
 */
char* get_obj_storage_type_string(obj_phys_t* obj) {
    switch (obj->o_type & OBJ_STORAGETYPE_MASK) {
        case OBJ_VIRTUAL:
            return "Virtual";
        case OBJ_EPHEMERAL:
            return "Ephemeral";
        case OBJ_PHYSICAL:
            return "Physical";
        default:
            return "(invalid type)";
    }
}

/**
 * Get a human-readable string that lists the type flags that are set on a given
 * object. This list does not include storage types; namely, it does not specify
 * whether the object is physical, virtual, or ephemeral.
 * 
 * obj:     A pointer to the object in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string. The caller must free
 *      this pointer when it is no longer needed.
 */
char* get_obj_type_flags_string(obj_phys_t* obj) {
    uint32_t flag_constants[] = {
        OBJ_NOHEADER,
        OBJ_ENCRYPTED,
        OBJ_NONPERSISTENT,
    };

    char* flag_strings[] = {
        "No-header",
        "Encrypted",
        "Non-persistent (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)",
    };

    // Allocate sufficient memory to store the longest possible string.
    size_t max_mem_required = 0;
    for (int i = 0; i < 3; i++) {
        max_mem_required += strlen(flag_strings[i]) + 2;
        // `+ 2` accounts for appending ", " to each string
    }
    char* result_string = malloc(max_mem_required);
    if (!result_string) {
        fprintf(stderr, "\nABORT: get_obj_type_flags_string: Could not allocate sufficient memory for `result_string`.\n");
    }
    
    char* cursor = result_string;

    // Go through possible flags. If a flag is set, append its corresponding
    // string to `result_string`. The flags are comma-delimited.
    for (int i = 0; i < 3; i++) {
        if (obj->o_type & flag_constants[i]) {
            if (cursor != result_string) {
                *cursor++ = ',';
                *cursor++ = ' ';
            }
            size_t flag_string_len = strlen(flag_strings[i]);
            memcpy(cursor, flag_strings[i], flag_string_len);
            cursor += flag_string_len;
        }
    }

    if (cursor == result_string) {
        // No strings were added, so it must be that no flags are set.
        char* no_flags_string = "(none)";
        size_t no_flags_string_len = strlen(no_flags_string);
        memcpy(cursor, no_flags_string, no_flags_string_len);
        cursor += no_flags_string_len;
    }

    *cursor = '\0';
    
    // Free up excess allocated memory.
    result_string = realloc(result_string, strlen(result_string) + 1);
    return result_string;
}

/**
 * Get a human-readable string describing a given `o_type` value. This is a
 * helper function for `get_obj_type_string()`, `get_obj_subtype_string()`,
 * and `print_checkpoint_mapping_string()`.
 * 
 * o_type:  A 32-bit bitfield whose lower 16 bits represent an APFS object type.
 *          Examples include the `o_type` field of `obj_phys_t`,
 *          and the `cpm_type` field of `checkpoint_mapping_t`.
 * 
 * RETURN VALUE:
 *      A pointer to the first character in the string.
 *      This pointer needn't be free by the caller.
 */
char* o_type_to_string(uint32_t o_type) {
    switch (o_type & OBJECT_TYPE_MASK) {
        case OBJECT_TYPE_NX_SUPERBLOCK:
            return "Container superblock";
        case OBJECT_TYPE_BTREE:
            return "B-tree (root node)";
        case OBJECT_TYPE_BTREE_NODE:
            return "B-tree (non-root) node";
        case OBJECT_TYPE_SPACEMAN:
            return "Space manager";
        case OBJECT_TYPE_SPACEMAN_CAB:
            return "Space manager chunk-info address block";
        case OBJECT_TYPE_SPACEMAN_CIB:
            return "Space manager chunk-info block";
        case OBJECT_TYPE_SPACEMAN_BITMAP:
            return "Space manager free-space bitmap";
        case OBJECT_TYPE_OMAP:
            return "Object map";
        case OBJECT_TYPE_CHECKPOINT_MAP:
            return "Checkpoint map";
        case OBJECT_TYPE_FS:
            return "APFS volume";
        case OBJECT_TYPE_NX_REAPER:
            return "Container reaper";
        case OBJECT_TYPE_NX_REAP_LIST:
            return "Container reaper list";
        case OBJECT_TYPE_EFI_JUMPSTART:
            return "EFI jumpstart boot info";
        case OBJECT_TYPE_NX_FUSION_WBC:
            return "Fusion device write-back cache state";
        case OBJECT_TYPE_NX_FUSION_WBC_LIST:
            return "Fusion device write-back cache list";
        case OBJECT_TYPE_ER_STATE:
            return "Encryption-rolling state";
        case OBJECT_TYPE_GBITMAP:
            return "General-purpose bitmap";
        case OBJECT_TYPE_GBITMAP_BLOCK:
            return "General purpose bitmap block";
        case OBJECT_TYPE_INVALID:
            return "(none/invalid)";
        case OBJECT_TYPE_TEST:
            return "A type reserved for testing (should never appear on disk --- if it does, file a bug against the APFS implementation that created this object)";
        case OBJECT_TYPE_CONTAINER_KEYBAG:
            return "Container keybag";
        case OBJECT_TYPE_VOLUME_KEYBAG:
            return "Volume keybag";
        default:
            return NULL;
    }
}

/**
 * Get a human-readable string describing a given `o_subtype` value.
 * This is a helper function for `get_obj_subtype_string()`
 * and `print_checkpoint_mapping_string()`.
 */

/**
 * Get a human-readable string describing a given `o_type` value. This is a
 * helper function for `get_obj_type_string()`, `get_obj_subtype_string()`,
 * and `print_checkpoint_mapping_string()`.
 * 
 * o_subtype:   A 32-bit field that represents an APFS object subtype.
 *              Examples include the `o_subtype` field of `obj_phys_t`,
 *              and the `cpm_subtype` field of `checkpoint_mapping_t`.
 * 
 * RETURN VALUE:
 *      A pointer to the first character in the string.
 *      This pointer needn't be free by the caller.
 */
char* o_subtype_to_string(uint32_t o_subtype) {
    // Check if `o_subtype` is a value representing a regular type.
    char* type_string = o_type_to_string(o_subtype);

    if (type_string) {
        return type_string;
    }

    // We didn't match against a regular type, so go through the
    // values that are exclusively used to represent subtypes.
    switch (o_subtype & OBJECT_TYPE_MASK) {
        case OBJECT_TYPE_SPACEMAN_FREE_QUEUE:
            return "Space manager free-space queue";
        case OBJECT_TYPE_EXTENT_LIST_TREE:
            return "Extents-list tree";
        case OBJECT_TYPE_FSTREE:
            return "File-system records tree";
        case OBJECT_TYPE_BLOCKREFTREE:
            return "Extent references tree";
        case OBJECT_TYPE_SNAPMETATREE:
            return "Volume snapshot metadata tree";
        case OBJECT_TYPE_OMAP_SNAPSHOT:
            return "Object map snapshots tree";
        case OBJECT_TYPE_FUSION_MIDDLE_TREE:
            return "Fusion inter-drive block-mapping tree";
        case OBJECT_TYPE_GBITMAP_TREE:
            return "B-tree of general-purpose bitmaps";
        default:
            return NULL;
    }
}

/**
 * Get a human-readable string with a given object's type.
 * 
 * obj:     A pointer to the object in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string; or `NULL` if the type
 *      is unrecognised. The caller needn't free this pointer.
 */
char* get_obj_type_string(obj_phys_t* obj) {
    return o_type_to_string(obj->o_type);
}

/**
 * Get a human-readable string with an object's subtype.
 * 
 * obj:     A pointer to the object in question.
 * 
 * RETURN VALUE:
 *      A pointer to the first character of the string; or `NULL` if the subtype
 *      is unrecognised. This pointer needn't be freed by the caller.
 */
char* get_obj_subtype_string(obj_phys_t* obj) {
    return o_subtype_to_string(obj->o_subtype);
}

/**
 * Print a nicely formatted string describing the data contained in the header
 * of an APFS object.
 */
void print_obj_hdr_info(obj_phys_t* obj) {
    char* type_flags_string = get_obj_type_flags_string(obj);
    
    bool malloced_type_string = false;
    char* type_string = get_obj_type_string(obj);
    if (!type_string) {
        char* format_string = "Unknown type (0x%08x) --- perhaps this type was introduced in a later version of APFS than that published on 2019-02-27.";
        size_t type_string_size = strlen(format_string) + 5; // Add 4 to account for `%08x` being replaced with an 8-digit hex number; add 1 more for terminating NULL byte.
        
        type_string = malloc(type_string_size);
        if (!type_string) {
            fprintf(stderr, "ABORT: print_obj_hdr_info: Could not allocate sufficient memory when generating `type_string` in the case where the object type is not recognised.\n");
            exit(-1);
        }
        malloced_type_string = true;

        sprintf(type_string, format_string, obj->o_type & OBJECT_TYPE_MASK);
    }

    bool malloced_subtype_string = false;
    char* subtype_string = get_obj_subtype_string(obj);
    if (!subtype_string) {
        char* format_string = "Unknown subtype (0x%08x) --- perhaps this subtype was introduced in a later version of APFS than that published on 2019-02-27.";
        size_t subtype_string_size = strlen(format_string) + 5; // Add 4 to account for `%08x` being replaced with an 8-digit hex number; add 1 more for terminating NULL byte.
        
        subtype_string = malloc(subtype_string_size);
        if (!subtype_string) {
            fprintf(stderr, "ABORT: print_obj_hdr_info: Could not allocate sufficient memory when generating `subtype_string` in the case where the object subtype is not recognised.\n");
            exit(-1);
        }
        malloced_subtype_string = true;

        sprintf(subtype_string, format_string, obj->o_subtype);
    }

    // Print the info
    printf("Stored checksum:    0x%016llx\n",   *(uint64_t*)obj);
    printf("OID:                0x%llx\n",      obj->o_oid);
    printf("XID:                0x%llx\n",      obj->o_xid);
    printf("Storage type:       %s\n",          get_obj_storage_type_string(obj));
    printf("Type flags:         %s\n",          type_flags_string);
    printf("Type:               %s\n",          type_string);
    printf("Subtype:            %s\n",          subtype_string);

    free(type_flags_string);
    if (malloced_type_string) {
        free(type_string);
    }
    if (malloced_subtype_string) {
        free(subtype_string);
    }
}

#endif // APFS_STRING_OBJECT_H
