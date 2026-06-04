#include <string>
#include <iostream>

class FriendList {
public:
  FriendList();
  void add(const std::string& name);
  size_t size() const;
  void set(size_t index, const std::string& name);

  friend std::ostream& operator<<(std::ostream& os, const FriendList& friend_list);

  ~FriendList();
  FriendList(const FriendList& friend_list);
  FriendList& operator=(FriendList friend_list);
  FriendList(FriendList&& friend_list) = delete;
  FriendList& operator=(FriendList&& friend_list) = delete;

private:
    std::string* _friends;
    size_t _size;
    size_t _capacity;

    void swap(FriendList& friend_list) noexcept;
};