#if not defined _UTILLIB_FILESYSTEM_
#define _UTILLIB_FILESYSTEM_

#if defined unix || defined __unix || defined __unix__
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#elif defined _WIN32
#include <Shlobj.h>
#endif

namespace utl
{
	auto home_dir() noexcept -> const char *
	{
		const char *res = nullptr;

#if defined unix || defined __unix || defined __unix__ // Unix way
		res = getenv("HOME");
		if ((res = getenv("HOME")) == nullptr)
			res = getpwuid(getuid())->pw_dir;
#elif defined _WIN32
		static_assert(false, "Home directory find isn't supported on windows.");
#endif

		return res;
	}
} // namespace utl

#endif