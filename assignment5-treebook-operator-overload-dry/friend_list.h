#include <string>
#include <iostream>
#include <vector>

class FriendList {
public:
  void add(const std::string& name);
  size_t size() const;
  void set(size_t index, const std::string& name);

  friend std::ostream& operator<<(std::ostream& os, const FriendList& friend_list);

private:
    std::vector<std::string> _friends;
};