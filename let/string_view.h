#ifndef LET_STRING_VIEW_H
#define LET_STRING_VIEW_H
namespace let
{
class StringView
{
  public:
    StringView(char *data, int length)
        : data_(data), length_(length)
    {
    }

  private:
    char *data_ = nullptr;
    int length_ = 0;
};
} // namespace let
#endif