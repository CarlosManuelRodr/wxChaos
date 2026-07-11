#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>
#include <angelscript.h>

int main(int argc, char** argv)
{
    if (asPrepareMultithread() < 0)
        return 1;

    doctest::Context context(argc, argv);
    const int result = context.run();

    asThreadCleanup();
    asUnprepareMultithread();
    return result;
}
