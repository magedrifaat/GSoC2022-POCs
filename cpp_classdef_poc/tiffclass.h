#include <iostream>

#include "cdef-object.h"
#include "cdef-manager.h"
#include "cdef-utils.h"
#include "cdef-class.h"

#include "defun.h"

OCTAVE_NAMESPACE_BEGIN

// TODO(maged): fix formatting to match code-base style

DEFMETHOD(tiff_constructor, interp, args, nargout, /*docs*/)
{
    octave_value_list retval;

    octave_value object = args(0);
    if (object.is_defined () && object.is_classdef_object ())
    {
        cdef_object& cdef_obj = to_cdef_ref(object);
        if (args.length() > 1)
            cdef_obj.put("counter", args(1));
        else
            cdef_obj.put("counter", octave_value(0));
        
        retval(0) = object;
    }

    return retval;
}

DEFMETHOD(tiff_print, interp, args, nargout, /* docs */)
{
    octave_value object = args(0);
    if (object.is_defined () && object.is_classdef_object ())
    {
        cdef_object& cdef_obj = to_cdef_ref(object);
        int counter = cdef_obj.get("counter").int_value();

        // TODO(maged): find the correct way of printing to octave
        std::cout<< "Counter = " << counter << std::endl;;
    }

    return octave_value_list();
}

DEFMETHOD(tiff_increment, interp, args, nargout, /* docs */)
{
    octave_value object = args(0);
    if (object.is_defined () && object.is_classdef_object ())
    {
        cdef_object& cdef_obj = to_cdef_ref(object);
        cdef_obj.put("counter", cdef_obj.get("counter").int_value() + 1);
    }

    return octave_value_list();
}

void create_tiffclass(cdef_manager& cdm, symbol_table& symtab)
{
    // The recipe for creating a classdef from C++ is as follows:
    //  1) Get a reference to the cdef_manager
    //  2) call make_class with the name of the class and a list
    //     of the superclasses to get back a cdef_class instance
    //  3) Set the relevant attributes of the cdef_class object if necessary
    //  4) For each property, call make_property to get back a cdef_property object
    //      which you can use to set the attributes and default value of necessary
    //  5) attach properties to the class using install_property method
    //  6) For each method, call make_method to get back a cdef_method object,
    //     which you can use to set the attributes of the method.
    //  7) attach the method to the class using install_method
    //  8) Get a reference to the symbol table and call install_built_in_function
    //     to add the constructor to the symbol table

    std::string class_name = "Tiff";

    std::list<cdef_class> slist;
    cdef_class cls = cdm.make_class(class_name, slist);
    
    cdef_property counter_prop = cdm.make_property(cls, "counter");
    cls.install_property(counter_prop);
    
    cdef_method ctor_meth = cdm.make_method(cls, class_name, Ftiff_constructor);
    cls.install_method(ctor_meth);

    cdef_method print_meth = cdm.make_method(cls, "print", Ftiff_print);
    cls.install_method(print_meth);

    cdef_method increment_meth = cdm.make_method(cls, "increment", Ftiff_increment);
    cls.install_method(increment_meth);

    symtab.install_built_in_function(class_name, cls.get_constructor_function());

    // TODO(maged): What is a dispatch? And why doesn't it work here?
    // symtab.install_built_in_dispatch("print", "Biff");

    // TODO(maged): checkout external functions?
    // TODO(maged): checkout operators and assignment(copy vs reference?)
}

OCTAVE_NAMESPACE_END