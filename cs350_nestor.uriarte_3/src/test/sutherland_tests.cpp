#include "common.hpp"

namespace cs350 {
    TEST(sutherland, basic)
    {
        // Configuration
        cs350::plane    plane({1, 0, 0}, {1, 0, 0});
        cs350::triangle tr;
        tr.points[0] = {0, 0, 0};
        tr.points[1] = {2, 1, 0};
        tr.points[2] = {0, 2, 0};

        // Split
        std::vector<cs350::triangle> positive, negative;
        cs350::split_triangle(tr, plane, positive, negative);
        ASSERT_EQ(positive.size(), 1u);
        ASSERT_EQ(negative.size(), 2u);

        // Split
        ASSERT_VEC_EQ(positive[0].points[0], {1, 0.5, 0});
        ASSERT_VEC_EQ(positive[0].points[1], {2, 1, 0});
        ASSERT_VEC_EQ(positive[0].points[2], {1, 1.5, 0});

        ASSERT_VEC_EQ(negative[0].points[0], {1, 0.5, 0});
        ASSERT_VEC_EQ(negative[0].points[1], {1, 1.5, 0});
        ASSERT_VEC_EQ(negative[0].points[2], {0, 2, 0});

        ASSERT_VEC_EQ(negative[1].points[0], {1, 0.5, 0});
        ASSERT_VEC_EQ(negative[1].points[1], {0, 2, 0});
        ASSERT_VEC_EQ(negative[1].points[2], {0, 0, 0});
    }

    TEST(sutherland, positive_side)
    {
        // Configuration
        cs350::plane    plane({1, 0, 0}, {-1, 0, 0});
        cs350::triangle tr;
        tr.points[0] = {0, 0, 0};
        tr.points[1] = {2, 1, 0};
        tr.points[2] = {0, 2, 0};

        // Split
        std::vector<cs350::triangle> positive, negative;
        cs350::split_triangle(tr, plane, positive, negative);
        ASSERT_EQ(positive.size(), 1u);
        ASSERT_EQ(negative.size(), 0u);

        // Split
        ASSERT_VEC_EQ(positive[0].points[0], {2, 1, 0});
        ASSERT_VEC_EQ(positive[0].points[1], {0, 2, 0});
        ASSERT_VEC_EQ(positive[0].points[2], {0, 0, 0});
    }

    TEST(sutherland, negative_side)
    {
        // Configuration
        cs350::plane    plane({1, 0, 0}, {3, 0, 0});
        cs350::triangle tr;
        tr.points[0] = {0, 0, 0};
        tr.points[1] = {2, 1, 0};
        tr.points[2] = {0, 2, 0};

        // Split
        std::vector<cs350::triangle> positive, negative;
        cs350::split_triangle(tr, plane, positive, negative);
        ASSERT_EQ(positive.size(), 0u);
        ASSERT_EQ(negative.size(), 1u);

        // Split
        ASSERT_VEC_EQ(negative[0].points[0], {2, 1, 0});
        ASSERT_VEC_EQ(negative[0].points[1], {0, 2, 0});
        ASSERT_VEC_EQ(negative[0].points[2], {0, 0, 0});
    }

    TEST(sutherland, positive_touching)
    {
        // Configuration
        cs350::plane    plane({1, 0, 0}, {0, 0, 0});
        cs350::triangle tr;
        tr.points[0] = {0, 0, 0};
        tr.points[1] = {2, 1, 0};
        tr.points[2] = {0, 2, 0};

        // Split
        std::vector<cs350::triangle> positive, negative;
        cs350::split_triangle(tr, plane, positive, negative);
        ASSERT_EQ(positive.size(), 1u);
        ASSERT_EQ(negative.size(), 0u);
    }

    TEST(sutherland, negative_touching)
    {
        // Configuration
        cs350::plane    plane({1, 0, 0}, {2, 0, 0});
        cs350::triangle tr;
        tr.points[0] = {0, 0, 0};
        tr.points[1] = {2, 1, 0};
        tr.points[2] = {0, 2, 0};

        // Split
        std::vector<cs350::triangle> positive, negative;
        cs350::split_triangle(tr, plane, positive, negative);
        ASSERT_EQ(positive.size(), 0u);
        ASSERT_EQ(negative.size(), 1u);
    }

    TEST(sutherland, centered)
    {
        // Configuration
        cs350::plane    plane({1, 0, 0}, {1, 0, 0});
        cs350::triangle tr;
        tr.points[0] = {0, 0, 0};
        tr.points[1] = {2, 0, 0};
        tr.points[2] = {1, 2, 0};

        // Split
        std::vector<cs350::triangle> positive, negative;
        cs350::split_triangle(tr, plane, positive, negative);
        ASSERT_EQ(positive.size(), 1u);
        ASSERT_EQ(negative.size(), 1u);
    }

    TEST(sutherland, real)
    {
        // Configuration
        cs350::plane    plane({0, 1, 0}, {-1.13, -3, 5});
        cs350::triangle tr{};
        tr.points = {{{-5.00, 5.00, 5.00}, {-5.00, -3.00, 3.00}, {-5.00, 5.00, -5.00}}};

        // Split
        std::vector<cs350::triangle> positive, negative;
        cs350::split_triangle(tr, plane, positive, negative);
        for(auto tri : positive) {
            std::cerr << tri << std::endl;
        }
        for(auto tri : negative) {
            std::cerr << tri << std::endl;
        }
    }
}