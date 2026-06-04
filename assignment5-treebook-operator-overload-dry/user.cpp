#include "user.h"

/**
 * Creates a new User with the given name and no friends.
 */
User::User(const std::string& name)
  : _name(name), _friends(FriendList{})
{
}

/**
 * Adds a friend to this User's list of friends.
 * @param name The name of the friend to add.
 */
void
User::add_friend(const std::string& name)
{
  _friends.add(name);
}

/**
 * Returns the name of this User.
 */
std::string
User::get_name() const
{
  return _name;
}

/**
 * Sets the friend at the given index to the given name.
 * @param index The index of the friend to set.
 * @param name The name to set the friend to.
 */
void User::set_friend(size_t index, const std::string& name)
{
  _friends.set(index, name);
}

std::ostream& operator<<(std::ostream& os, const User& user) {
  os << "User(name=" << user._name << ", friends=";
  os << user._friends;
  os << ")";
  return os;
}