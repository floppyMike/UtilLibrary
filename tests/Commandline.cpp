#include <gtest/gtest.h>
#include <Util/Commandline.h>
#include <Util/Error.h>

TEST(Commandline, Basic)
{
	const char *ar[] = { "./exec", "-aabcAss", "Bruh", "--ass", "ABC" };

	uint32_t	a = 0;
	uint32_t	b = 0;
	std::string c;
	std::string d;
	std::string ass;
	arg_to_data(5, ar,
				utl::Arguments<1, 1, 1, 2>{ .optional_names		   = { "ass" },
											.optional_short_names  = { 'c' },
											.flag_names			   = { 'a', 'b' },
											.required_values	   = { &d },
											.optional_values	   = { &ass },
											.optional_short_values = { &c },
											.flag_values		   = { &a, &b } });

	ASSERT_EQ(a, 2);
	ASSERT_EQ(b, 1);
	ASSERT_EQ(c, "Ass");
	ASSERT_EQ(d, "Bruh");
	ASSERT_EQ(ass, "ABC");
}

auto main(int argc, char **argv) -> int
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}