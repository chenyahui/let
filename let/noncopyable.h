#ifndef LET_NONCOPYABLE_H
#define LET_NONCOPYABLE_H
namespace let
{
class NonCopyAble
{
  public:
    NonCopyAble() = default;

  private:
    NonCopyAble(const NonCopyAble &) = delete;
    NonCopyAble &operator=(const NonCopyAble &) = delete;

    NonCopyAble(NonCopyAble &&) = delete;
    NonCopyAble &operator=(NonCopyAble &&) = delete;
};
} // namespace let
#endif