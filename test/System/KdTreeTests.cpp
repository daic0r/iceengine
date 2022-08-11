#include <gtest/gtest.h>
#include <System/KdTree.h>
#include <Entities/Entity.h>

TEST(KdTreeTests, Construct) {
    Ice::KdTree<Ice::Entity> kd{ { 3.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, 10.0f, 10.0f, 1.0f } };

    kd.print();

    EXPECT_EQ(1, 1);
}