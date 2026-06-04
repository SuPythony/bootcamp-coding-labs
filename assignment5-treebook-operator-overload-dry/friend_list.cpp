#include "friend_list.h"

/**
 * Adds a friend to this User's list of friends.
 * @param name The name of the friend to add.
 */
void
FriendList::add(const std::string& name)
{
  _friends.push_back(name);
}

/**
 * Returns the number of friends this User has.
 */
size_t
FriendList::size() const
{
  return _friends.size();
}

/**
 * Sets the friend at the given index to the given name.
 * @param index The index of the friend to set.
 * @param name The name to set the friend to.
 */
void FriendList::set(size_t index, const std::string& name)
{
  _friends[index] = name;
}

std::ostream& operator<<(std::ostream& os, const FriendList& friend_list) {
  os << "[";
  for (int i = 0; i < friend_list.size(); i++) {
      os << friend_list._friends[i];
      if (i < friend_list.size() - 1) os << ", ";
  }
  os << "]";
  return os;
}