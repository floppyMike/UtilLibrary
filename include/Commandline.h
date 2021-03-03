#pragma once

#include <string_view>
#include <iostream>
#include <vector>
#include <array>
#include "Parse.h"

template<size_t required_n, size_t optional_n, size_t short_optional_n, size_t flag_n>
struct Arguments
{
	std::array<std::string_view, optional_n> optional_names;
	std::array<char, short_optional_n>		 optional_short_names;
	std::array<char, flag_n>				 flag_names;

	std::array<std::string *, required_n>		required_values;
	std::array<std::string *, optional_n>		optional_values;
	std::array<std::string *, short_optional_n> optional_short_values;
	std::array<uint32_t *, flag_n>				flag_values;
};

template<size_t n0, size_t n1, size_t n2, size_t n3>
void arg_to_data(int argc, const char *const *argv, Arguments<n0, n1, n2, n3> &&k)
{
	++argv; // Skip executable loc
	--argc;

	auto inbound = [argv, argc](auto i, const char *str) { // Cancel on missing arguments
		if (++i == argv + argc)
			throw std::out_of_range(str);
		return i;
	};

	auto req_i = k.required_values.begin();

	for (auto i = argv; i != argv + argc; ++i)
	{
		SequentialParser p(*i);

		// Is REQUIRED
		if (p.current() != '-' || p.total_size() == 1) // No '-' or size to small for option -> required
		{
			if (req_i == k.required_values.end()) // Check for overflow
				throw std::out_of_range("More arguments than expected.");

			**req_i++ = p.rest(); // Store required

			continue;
		}

		p.skip_for(1); // Skip the '-'

		// Is OPTIONAL
		if (p.current() == '-') // "--" must be
		{
			p.skip_for(1); // Skip the second '-'

			const auto opt = std::find(k.optional_names.begin(), k.optional_names.end(),
									   p.rest()); // Find if string start with option name.

			if (opt == k.optional_names.end()) // Error on not found
				throw std::logic_error("Option not found.");

			const auto v = *(i = inbound(i, "Missing option value.")); // Get value from next to string or next string

			*k.optional_values[std::distance(k.optional_names.begin(), opt)] = v; // Save the value
			continue;
		}

		while (!p.at_end()) // Go through each character
		{
			const auto ch = p.get();

			// Is FLAG
			if (const auto f = std::find(k.flag_names.begin(), k.flag_names.end(), ch); f != k.flag_names.end())
			{
				++*k.flag_values[std::distance(k.flag_names.begin(), f)];
				continue;
			}

			// Is OPTIONAL SHORT
			const auto c = std::find(k.optional_short_names.begin(), k.optional_short_names.end(), ch);

			if (c == k.optional_short_names.end())
				throw std::logic_error("Option not found.");

			const auto v = p.at_end() ? *(i = inbound(i, "Missing option value."))
									  : p.rest(); // Collect its value from next to string or next string

			*k.optional_short_values[std::distance(k.optional_short_names.begin(), c)] = v; // Save the value
			break;
		}
	}

	if (req_i != k.required_values.end()) // Check if all necessary commands gotten
		throw std::runtime_error("Missing required values.");
}