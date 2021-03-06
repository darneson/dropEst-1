#include "UtilFunctions.h"

#include <cstdlib>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>


namespace Tools
{
	double fpow(double base, long exp)
	{
		if (exp == 1)
			return base;

		double result = 1;
		while (exp)
		{
			if (exp & 1)
			{
				result *= base;
			}
			exp >>= 1;
			base *= base;
		}

		return result;
	}

	unsigned edit_distance(const char *s1, const char *s2, bool skip_n, unsigned max_ed)
	{
		int olddiag;
		int s1len = strlen(s1);
		int s2len = strlen(s2);
		int column[s1len + 1];
		for (int s1_ind = 0; s1_ind <= s1len; s1_ind++)
		{
			column[s1_ind] = s1_ind;
		}

		for (int s2_ind = 1; s2_ind <= s2len; s2_ind++)
		{
			int lower_index = std::max(0, s2_ind - int(max_ed));
			int upper_index = std::min(s1len, s2_ind + int(max_ed));
			int lastdiag = column[lower_index];
			column[lower_index] = s2_ind;
			int min_ed = s2_ind;
			for (int s1_ind = lower_index + 1; s1_ind <= upper_index; s1_ind++)
			{
				olddiag = column[s1_ind];
				bool is_match = (s1[s1_ind - 1] == s2[s2_ind - 1]) || (skip_n && (s1[s1_ind - 1] == 'N' || s2[s2_ind - 1] == 'N'));
				int new_ed = MIN3(column[s1_ind] + 1, column[s1_ind - 1] + 1, lastdiag + int(!is_match));
				min_ed = std::min(min_ed, new_ed + std::abs(s1_ind - s2_ind));
				column[s1_ind] = new_ed;
				lastdiag = olddiag;
			}

			if (min_ed > max_ed)
				return unsigned(min_ed);
		}

		return unsigned(column[s1len]);
	}

	unsigned hamming_distance(const std::string &s1, const std::string &s2, bool skip_n)
	{
		if (s1.size() != s2.size())
			throw std::runtime_error("Strings should have equal length");

		unsigned ed = 0;
		for (std::string::size_type i = 0; i < s1.size(); ++i)
		{
			if (s1[i] != s2[i] && !(skip_n && (s1[i] == 'N' || s2[i] == 'N')))
			{
				++ed;
			}
		}

		return ed;
	}

	RInside* init_r()
	{
		RInside *r = RInside::instancePtr();
		if (r == nullptr)
		{
			r = new RInside(0, nullptr);
		}

		r->parseEvalQ("library(Matrix)");

		return r;
	}

	ReverseComplement::ReverseComplement()
	{
		this->complements['A'] = 'T';
		this->complements['T'] = 'A';
		this->complements['G'] = 'C';
		this->complements['C'] = 'G';
		this->complements['N'] = 'N';
	}

	std::string ReverseComplement::rc(const std::string &s) const
	{
		std::string res = s;
		for (int i = 0; i < s.length(); i++)
		{
			res[i] = this->complements[s[s.length() - i - 1]];
		}

		return res;
	}

	std::string ltrim(const std::string &str)
	{
		if (str.empty())
			return str;

		return str.substr(str.find_first_not_of(" \t"));
	}

	std::string expand_tilde_in_path(const std::string &path)
	{
		if (path.length() < 2)
			return path;

		if (path.substr(0, 2) == "~/")
			return std::getenv("HOME") + path.substr(1);

		return path;
	}

	void copy_file(const std::string &src_file, const std::string &dest_file)
	{
		std::ifstream src(src_file, std::ios::binary);
		std::ofstream dest(dest_file, std::ios::binary);
		dest << src.rdbuf();
	}

	std::string expand_relative_path(const std::string &src_fname, const std::string &rel_fname)
	{
		if (rel_fname.empty() || rel_fname[0] == '/')
			return rel_fname;

		return boost::filesystem::path(src_fname).parent_path().append(rel_fname, boost::filesystem::path::codecvt()).string();
	}
}

