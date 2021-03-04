#include <gtest/gtest.h>
#include <Util/Graph.h>

// -----------------------------------------------------------------------------
// Data
// -----------------------------------------------------------------------------

static constexpr utl::Edge nodes1[] = { { 1 }, { 2 }, { 1 }, { 3 }, { 5 }, { 2 }, { 4 }, { 5 } };
static constexpr size_t		map1[]	 = { 0, 1, 2, 5, 7, 8, 8 };

static constexpr utl::Graph<const utl::Edge *, const size_t *> g1{ .edges = nodes1,
																	 .idx	= map1,
																	 .size	= sizeof(map1) / sizeof(map1[0]) - 1 };

static constexpr utl::WeighedEdge nodes2[] = {
	{ 1, 4 }, { 7, 8 },	 { 7, 11 }, { 2, 8 }, { 8, 2 }, { 5, 4 }, { 3, 7 },
	{ 4, 9 }, { 5, 14 }, { 5, 10 }, { 6, 2 }, { 8, 6 }, { 7, 1 }, { 8, 7 }
};
static constexpr size_t map2[] = { 0, 2, 4, 7, 9, 10, 11, 13, 14, 14 };

static constexpr utl::Graph<const utl::WeighedEdge *, const size_t *> g2{
	.edges = nodes2, .idx = map2, .size = sizeof(map2) / sizeof(map2[0]) - 1
};

// -----------------------------------------------------------------------------
// Tests
// -----------------------------------------------------------------------------

TEST(Graph, Breadth_First_Search_Basic)
{
	constexpr size_t comp[] = { 0, 0, 1, 2, 3, 2 };
	const auto		 res	= utl::breadth_first_search(g1, 0);
	for (size_t i = 0; i < res.size(); ++i) EXPECT_EQ(comp[i], res[i]) << "At index " << i;
}

TEST(Graph, Breadth_First_Search_Complex)
{
	constexpr size_t comp[] = { 0, 0, 1, 2, 3, 2, 5, 0, 7 };
	const auto		 res	= utl::breadth_first_search(g2, 0);
	for (size_t i = 0; i < res.size(); ++i) EXPECT_EQ(comp[i], res[i]) << "At index " << i;
}

TEST(Graph, Dijkstra_Search)
{
	constexpr size_t comp[]	  = { 0, 0, 1, 2, 3, 2, 5, 0, 2 };
	constexpr size_t weight[] = { 0, 4, 12, 19, 28, 16, 18, 8, 14 };
	const auto		 res	  = utl::dijkstra_search(g2, 0);
	for (size_t i = 0; i < res.first.size(); ++i)
	{
		EXPECT_EQ(comp[i], res.first[i]) << "At index " << i;
		EXPECT_EQ(weight[i], res.second[i]) << "At index " << i;
	}
}

TEST(Graph, A_Star)
{
	// constexpr size_t comp[]	  = { 0, 0, 1, 2, 3, 2, 5, 0, 2 };
	// constexpr size_t weight[] = { 0, 4, 12, 19, 28, 16, 18, 8, 14 };
	// const auto		 res	  = utl::a_star(g2, 0, );
	// for (size_t i = 0; i < res.first.size(); ++i)
	// {
	// 	EXPECT_EQ(comp[i], res.first[i]) << "At index " << i;
	// 	EXPECT_EQ(weight[i], res.second[i]) << "At index " << i;
	// }
}

auto main(int argc, char **argv) -> int
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}