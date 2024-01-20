/*
 * reversed code, licensed under MIT license.
 *
 * $Id: winwrap.c 1.2 2023/06/15 20:14:27 Exp $ chry
 * $Locker: $
 *
 * Generic name conversion function
 */
#include "brwrap.h"
#include <string.h>

typedef struct brw_event_names
{
    br_uint_16 type;
    const char* name;
} brw_event_names;

#define QUALIFIERS_NAMES_MAX 6
#define EVENT_NAMES_MAX 12
#define KEYNAMES_MAX 63

static brw_event_names qualifierNames_S3154[QUALIFIERS_NAMES_MAX] = {
    {.type = BRW_QUAL_SHIFT, .name = "SHIFT" },
    {.type = BRW_QUAL_CONTROL, .name = "CONTROL" },
    {.type = BRW_QUAL_ALT, .name = "ALT" },
    {.type = BRW_QUAL_POINTER_1, .name = "POINTER1" },
    {.type = BRW_QUAL_POINTER_2, .name = "POINTER2" },
    {.type = BRW_QUAL_POINTER_3, .name = "POINTER3" },
};

static brw_event_names eventNames_S3080[EVENT_NAMES_MAX] = {
    {.type = BRW_EVENT_KEY_DOWN, .name = "KEY_DOWN" },
    {.type = BRW_EVENT_KEY_UP, .name = "KEY_UP" },
    {.type = BRW_EVENT_POINTER1_DOWN, .name = "POINTER1_DOWN" },
    {.type = BRW_EVENT_POINTER1_UP, .name = "POINTER1_UP" },
    {.type = BRW_EVENT_POINTER2_DOWN, .name = "POINTER2_DOWN" },
    {.type = BRW_EVENT_POINTER2_UP, .name = "POINTER2_UP" },
    {.type = BRW_EVENT_POINTER3_DOWN, .name = "POINTER3_DOWN" },
    {.type = BRW_EVENT_POINTER3_UP, .name = "POINTER3_UP" },
    {.type = BRW_EVENT_POINTER_MOVE, .name = "POINTER_MOVE" },
    {.type = BRW_EVENT_TIMER, .name = "TIMMER" },
    {.type = BRW_EVENT_COMMAND, .name = "COMMAND" },
    {.type = BRW_EVENT_CHAR, .name = "KEY_DOWN" },
};

static brw_event_names keyNames_S3063[KEYNAMES_MAX] = {
    {.type = BRW_KEY_NONE, .name = "NONE" },

    {.type = BRW_KEY_SHIFT, .name = "SPACE" },
    {.type = BRW_KEY_CONTROL, .name = "CONTROL" },
    {.type = BRW_KEY_ALT, .name = "ALT" },

    {.type = BRW_KEY_TAB, .name = "TAB" },
    {.type = BRW_KEY_BACKSPACE, .name = "BACKSPACE" },

    {.type = BRW_KEY_CANCEL, .name = "CANCEL" },
    {.type = BRW_KEY_SELECT, .name = "SELECT" },

    {.type = BRW_KEY_UP, .name = "UP" },
    {.type = BRW_KEY_DOWN, .name = "DOWN" },
    {.type = BRW_KEY_LEFT, .name = "LEFT" },
    {.type = BRW_KEY_RIGHT, .name = "RIGHT" },

    {.type = BRW_KEY_FIRST, .name = "FIRST" },
    {.type = BRW_KEY_LAST, .name = "LAST" },

    {.type = BRW_KEY_PREV, .name = "PREV" },
    {.type = BRW_KEY_NEXT, .name = "NEXT" },

    {.type = BRW_KEY_SPACE, .name = "SPACE" },

    {.type = BRW_KEY_0, .name = "0" },
    {.type = BRW_KEY_1, .name = "1" },
    {.type = BRW_KEY_2, .name = "2" },
    {.type = BRW_KEY_3, .name = "3" },
    {.type = BRW_KEY_4, .name = "4" },
    {.type = BRW_KEY_5, .name = "5" },
    {.type = BRW_KEY_6, .name = "6" },
    {.type = BRW_KEY_7, .name = "7" },
    {.type = BRW_KEY_8, .name = "8" },
    {.type = BRW_KEY_9, .name = "9" },

    {.type = BRW_KEY_A, .name = "A" },
    {.type = BRW_KEY_B, .name = "B" },
    {.type = BRW_KEY_C, .name = "C" },
    {.type = BRW_KEY_D, .name = "D" },
    {.type = BRW_KEY_E, .name = "E" },
    {.type = BRW_KEY_F, .name = "F" },
    {.type = BRW_KEY_G, .name = "G" },
    {.type = BRW_KEY_H, .name = "H" },
    {.type = BRW_KEY_I, .name = "I" },
    {.type = BRW_KEY_J, .name = "J" },
    {.type = BRW_KEY_K, .name = "K" },
    {.type = BRW_KEY_L, .name = "L" },
    {.type = BRW_KEY_M, .name = "M" },
    {.type = BRW_KEY_N, .name = "N" },
    {.type = BRW_KEY_O, .name = "O" },
    {.type = BRW_KEY_P, .name = "P" },
    {.type = BRW_KEY_Q, .name = "Q" },
    {.type = BRW_KEY_R, .name = "R" },
    {.type = BRW_KEY_S, .name = "S" },
    {.type = BRW_KEY_T, .name = "T" },
    {.type = BRW_KEY_U, .name = "U" },
    {.type = BRW_KEY_V, .name = "V" },
    {.type = BRW_KEY_W, .name = "W" },
    {.type = BRW_KEY_X, .name = "X" },
    {.type = BRW_KEY_Y, .name = "Y" },
    {.type = BRW_KEY_Z, .name = "Z" },


    {.type = BRW_KEY_F1, .name = "F1" },
    {.type = BRW_KEY_F2, .name = "F2" },
    {.type = BRW_KEY_F3, .name = "F3" },
    {.type = BRW_KEY_F4, .name = "F4" },
    {.type = BRW_KEY_F5, .name = "F5" },
    {.type = BRW_KEY_F6, .name = "F6" },
    {.type = BRW_KEY_F7, .name = "F7" },
    {.type = BRW_KEY_F8, .name = "F8" },
    {.type = BRW_KEY_F9, .name = "F9" },
    {.type = BRW_KEY_F10,.name = "F10" },

};

/* not present in the reverse, but we cannot access internal msvc functions... */
static br_boolean is_unicode(br_int_32 v)
{
    return((0x30a0 <= v) && (v <= 0x30ff));
}

static br_int_32 NameFromValue(char* buffer, br_uint_16 type, brw_event_names* array, int array_len)
{
    int i;

    for (i = 0; i < array_len; i++)
    {
        if (array[i].type == type)
            break;
    }

    if (i >= array_len)
        return BrSprintf(buffer, "?%d?", type);

    return BrSprintf(buffer, "%s", array[i].name);
}


char* BrwEventText(char* dest, brw_event* e)
{
    br_int_32 pos = NameFromValue(dest, e->type, eventNames_S3080, EVENT_NAMES_MAX);
    char* buffer = dest + pos;
    int i;

    *buffer = '(';
    buffer++;

    if (e->qualifiers)
    {
        *buffer = '[';
        buffer++;

        for (i = 0; i < QUALIFIERS_NAMES_MAX; i++)
        {
            if (e->qualifiers & (qualifierNames_S3154[i].type))
            {
                buffer += BrSprintf(buffer, "%s", qualifierNames_S3154[i].name);
            }
        }

        *buffer = ']';
        buffer++;
    }
    else
    {
        buffer += BrSprintf(buffer, "%s", "[NONE]");
    }

    *buffer = ',';
    buffer++;

    switch (e->type)
    {
    case BRW_EVENT_KEY_DOWN:
    case BRW_EVENT_KEY_UP:
    {
        pos = NameFromValue(buffer, e->value_1, keyNames_S3063, KEYNAMES_MAX);
        buffer += pos;
        *buffer = ',';
        buffer++;

        if (is_unicode(e->value_2))
            buffer += BrSprintf(buffer, "%02x", e->value_2);
        else
            buffer += BrSprintf(buffer, "'%c'", e->value_2);

        break;
    }

    case BRW_EVENT_POINTER1_DOWN:
    case BRW_EVENT_POINTER1_UP:
    case BRW_EVENT_POINTER2_DOWN:
    case BRW_EVENT_POINTER2_UP:
    case BRW_EVENT_POINTER3_DOWN:
    case BRW_EVENT_POINTER3_UP:
    case BRW_EVENT_POINTER_MOVE:
    case BRW_EVENT_TIMER:
        buffer += BrSprintf(buffer, "%d,%d", e->value_1, e->value_2);
        break;
    case BRW_EVENT_COMMAND:
        buffer += BrSprintf(buffer, "%d", e->value_1);
        break;
    case BRW_EVENT_CHAR:
    {
        if (is_unicode(e->value_2))
            buffer += BrSprintf(buffer, "%02x", e->value_2);
        else
            buffer += BrSprintf(buffer, "'%c'", e->value_2);

        break;
    }
    default:
        buffer += BrSprintf(buffer, "0x%x,0x%x", e->value_1, e->value_2);
        break;
    }

    *buffer = ')';
    buffer++;
    return buffer;
}

