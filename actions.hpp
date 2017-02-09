#pragma once

#include <utility>
#include <memory>
#include <functional>
#include "utils.hpp"
#include "types.hpp"

namespace phosphor
{
namespace inventory
{
namespace manager
{

class Manager;

using Action = std::function<void (sdbusplus::bus::bus&, Manager&)>;

/** @brief make_action
 *
 *  Adapt an action function object.
 *
 *  @param[in] action - The action being adapted.
 *  @returns - The adapted action.
 *
 *  @tparam T - The type of the action being adapted.
 */
template <typename T>
auto make_action(T&& action)
{
    return Action(std::forward<T>(action));
}

namespace actions
{

/** @brief Destroy objects action.  */
inline auto destroyObjects(std::vector<const char*>&& paths)
{
    return [ = ](auto&, auto & m)
    {
        m.destroyObjects(paths);
    };
}

/** @brief Create objects action.  */
inline auto createObjects(
    std::map<sdbusplus::message::object_path, Object>&& objs)
{
    return [ = ](auto&, auto & m)
    {
        m.createObjects(objs);
    };
}

/** @brief Set a property action.
 *
 *  Invoke the requested method with a reference to the requested
 *  sdbusplus server binding interface as a parameter.
 *
 *  @tparam T - The sdbusplus server binding interface type.
 *  @tparam U - The type of the sdbusplus server binding member
 *      function that sets the property.
 *  @tparam V - The property value type.
 *
 *  @param[in] path - The DBus path on which the property should
 *      be set.
 *  @param[in] iface - The DBus interface hosting the property.
 *  @param[in] member - Pointer to sdbusplus server binding member.
 *  @param[in] value - The value the property should be set to.
 *
 *  @returns - A function object that sets the requested property
 *      to the requested value.
 */
template <typename T, typename U, typename V>
auto setProperty(
    const char* path, const char* iface,
    U&& member, V&& value)
{
    // The manager is the only parameter passed to actions.
    // Bind the path, interface, interface member function pointer,
    // and value to a lambda.  When it is called, forward the
    // path, interface and value on to the manager member function.
    return [path, iface, member,
                  value = std::forward<V>(value)](auto&, auto & m)
    {
        m.template invokeMethod<T>(
            path, iface, member, value);
    };
}

} // namespace actions
} // namespace manager
} // namespace inventory
} // namespace phosphor

// vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4
