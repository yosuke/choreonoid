/*!
 * @author Hisashi Ikari
 */
#include <boost/python.hpp>

// This header is an "EXPLICIT" item of dependence.
// And this items are located in the "cnoid/include" or same directory.
// Its meaning is exposed externally. For reason, we can will be selected here.

#include <cnoid/Referenced>
#include <cnoid/Item>
#include <cnoid/RootItem>
#include <cnoid/WorldItem>
#include <cnoid/BodyItem>
#include <cnoid/ControllerItem>

#include "../../Base/python/PyBase.h"
#include "../SimpleControllerItem.h"

// We recommend the use of minieigen.
using namespace boost::python;
using namespace cnoid;

namespace cnoid
{
    /*!
     * @brief Reference types are explicitly declare a function pointer. 
     *        Reference types share a reference to the original(in C++).
     *        But, it does not share the type of destination(in python).
     *        (Reason-1. Primitive types can not be a reference type.)
     *        (Reason-2. Minieigen create an object always.)
     *        Therefore, we must use the setter if you want to use the python.
     *        We will define the settings of the following variables.       
     */
    BOOST_PYTHON_MODULE(SimpleControllerPlugin)
    {
        /*!
         * @brief Define the interface for SimpleControllerItem.
         */
        class_ < SimpleControllerItem, ref_ptr<SimpleControllerItem>, 
            bases<Item>, boost::noncopyable >("SimpleControllerItem", init<>())
            .def("__init__", boost::python::make_constructor(&createInstance<SimpleControllerItem>))
            .def("setControllerDllName", &SimpleControllerItem::setControllerDllName);

    }

}; // end of namespace
