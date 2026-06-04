#include "friend_list.h"

/**
 * Adds a friend to this User's list of friends.
 * @param name The name of the friend to add.
 */
void
FriendList::add(const std::string& name)
{
  if (_size == _capacity) {
    _capacity = 2 * _capacity + 1;
    std::string* newFriends = new std::string[_capacity];
    for (size_t i = 0; i < _size; ++i) {
      newFriends[i] = _friends[i];
    }
    delete[] _friends;
    _friends = newFriends;
  }

  _friends[_size++] = name;
}

/**
 * Returns the number of friends this User has.
 */
size_t
FriendList::size() const
{
  return _size;
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
  for (int i = 0; i < friend_list._size; i++) {
      os << friend_list._friends[i];
      if (i < friend_list._size - 1) os << ", ";
  }
  os << "]";
  return os;
}

FriendList::FriendList() : _size{0}, _capacity{0}, _friends{nullptr} {}

FriendList::~FriendList() {
  delete[] _friends;
}

FriendList::FriendList(const FriendList& friend_list) : _friends{new std::string[friend_list._capacity]},
                                                        _size{friend_list._size}, _capacity{friend_list._capacity}
{
  for (int i = 0; i < _size; i++) {
    _friends[i] = friend_list._friends[i];
  }
}

FriendList& FriendList::operator=(FriendList friend_list) {
  if (this == &friend_list) return *this;
  swap(friend_list);
  return *this;
}

void FriendList::swap(FriendList& friend_list) noexcept {
  std::swap(_size, friend_list._size);
  std::swap(_capacity, friend_list._capacity);
  std::swap(_friends, friend_list._friends);
}