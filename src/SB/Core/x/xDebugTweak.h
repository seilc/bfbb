#ifndef XDEBUGTWEAK_H
#define XDEBUGTWEAK_H

#include <types.h>

struct tweak_info;

struct tweak_callback
{
    typedef void(*event_callback)(const tweak_info&);
    typedef void(*convert_callback)(const tweak_info&, void*);

    event_callback on_change;
    event_callback on_select;
    event_callback on_unselect;
    event_callback on_start_edit;
    event_callback on_stop_edit;
    event_callback on_expand;
    event_callback on_collapse;
    event_callback on_update;
    convert_callback convert_mem_to_tweak;
    convert_callback convert_tweak_to_mem;

    // UNUSED: needed to match (code from Rat GC, RotU GC, Incredibles GC)
    static tweak_callback create(event_callback on_change, event_callback on_select = NULL, event_callback on_unselect = NULL, event_callback on_start_edit = NULL, event_callback on_stop_edit = NULL, event_callback on_expand = NULL, event_callback on_collapse = NULL, event_callback on_update = NULL, convert_callback convert_mem_to_tweak = NULL, convert_callback convert_tweak_to_mem = NULL)
    {
        tweak_callback cb = { on_change, on_select, on_unselect, on_start_edit, on_stop_edit,
                              on_expand, on_collapse, on_update,
                              convert_mem_to_tweak, convert_tweak_to_mem };
        return cb;
    }

    static tweak_callback create_change(event_callback on_change)
    { tweak_callback cb = { on_change, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL }; return cb; }

    // UNUSED: needed to match (speculative code)
    static tweak_callback create_select(event_callback on_select, event_callback on_unselect)
    { tweak_callback cb = { NULL, on_select, on_unselect, NULL, NULL, NULL, NULL, NULL, NULL, NULL }; return cb; }

    // UNUSED: needed to match (code from Rat GC, RotU GC)
    static tweak_callback create_edit(event_callback on_start_edit, event_callback on_stop_edit)
    { tweak_callback cb = { NULL, NULL, NULL, on_start_edit, on_stop_edit, NULL, NULL, NULL, NULL, NULL }; return cb; }

    // UNUSED: needed to match (code from Rat GC)
    static tweak_callback create_expand(event_callback on_expand, event_callback on_collapse)
    { tweak_callback cb = { NULL, NULL, NULL, NULL, NULL, on_expand, on_collapse, NULL, NULL, NULL }; return cb; }

    // UNUSED: needed to match (code from Rat GC)
    static tweak_callback create_update(event_callback on_update)
    { tweak_callback cb = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, on_update, NULL, NULL }; return cb; }

    // UNUSED: needed to match (code from Rat GC)
    static tweak_callback create_converter(convert_callback convert_mem_to_tweak, convert_callback convert_tweak_to_mem)
    { tweak_callback cb = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, convert_mem_to_tweak, convert_tweak_to_mem }; return cb; }
};

#endif
