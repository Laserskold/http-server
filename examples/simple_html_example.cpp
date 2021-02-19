#include "html/html.h"

int main(int, char *[]) {
    auto html = Html{
        .tag = "html",
        .attributes = {{"version", "1"}},
        .children =
            {
                {
                    .tag = "head",
                    .attributes = {},
                    .children =
                        {
                            {
                                .tag = "title",
                                .attributes = {},
                                .children = {},
                                .content = "Hello",
                            },
                        },
                },
            },
    };

    // This can be sent with the http server as well
    std::cout << html << "\n";

    return 0;
}
