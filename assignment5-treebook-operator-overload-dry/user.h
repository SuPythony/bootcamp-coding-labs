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

  ~User() = default;
  User(const User& user) = default;
  User& operator=(const User& user) = default;
  User(User&& user) = delete;
  User& operator=(User&& user) = delete;

private:
  std::string _name;
  FriendList _friends;
};