/*
 * Assignment 4: TreeBook
 * Adapted by Tinkercademy from Stanford CS106L Assignment 5
 * (originally by Fabio Ibanez, with modifications by Jacob Roberts-Baca).
 */

#include <iostream>
#include <string>
#include "friend_list.h"

class User
{
public:
  User(const std::string& name);
  void add_friend(const std::string& name);
  std::string get_name() const;
  void set_friend(size_t index, const std::string& name);

  friend std::ostream& operator<<(std::ostream& os, const User& user);

  /*
  Removing the move constructor and assignment fails the tests as it requires
  User to be non-movable. So if I just delete those 2, by rule of 5 I'll need
  the rest of the SMF's too. Therefore, keeping them all.
  */
  ~User() = default;
  User(const User& user) = default;
  User& operator=(const User& user) = default;
  User(User&& user) = delete;
  User& operator=(User&& user) = delete;

  User& operator+=(User& rhs);
  bool operator<(const User& rhs) const;

private:
  std::string _name;
  FriendList _friends;
};