/**
 * @file symbol_container_imp.cpp
 * Internal container for symbols
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Philippe Elie
 * @author John Levon
 */

#include <string>
#include <algorithm>
#include <set>
#include <vector>

#include "symbol_functors.h"
#include "symbol_container_imp.h"
#include "profile_container.h"

using namespace std;

symbol_index_t symbol_container_imp_t::size() const
{
	return symbols.size();
}

symbol_entry const & symbol_container_imp_t::operator[](symbol_index_t index) const
{
	return symbols[index];
}

void symbol_container_imp_t::push_back(symbol_entry const & symbol)
{
	symbols.push_back(symbol);
}

symbol_entry const *
symbol_container_imp_t::find(string filename, size_t linenr) const
{
	build_by_file_loc();

	symbol_entry symbol;
	symbol.sample.file_loc.filename = filename;
	symbol.sample.file_loc.linenr = linenr;

	set_symbol_by_file_loc::const_iterator it =
		symbol_entry_by_file_loc.find(&symbol);

	if (it != symbol_entry_by_file_loc.end())
		return *it;

	return 0;
}

vector<symbol_entry const *>
symbol_container_imp_t::find(string name) const
{
	vector<symbol_entry const *> v;

	vector<symbol_entry>::const_iterator it;
	for (it = symbols.begin() ; it != symbols.end() ; ++it) {
		if (it->name == name)
			v.push_back(&*it);
	}

	return v;
}

void  symbol_container_imp_t::build_by_file_loc() const
{
	if (symbols.size() && symbol_entry_by_file_loc.empty()) {
		for (symbol_index_t i = 0 ; i < symbols.size() ; ++i)
			symbol_entry_by_file_loc.insert(&symbols[i]);
	}
}

symbol_entry const * symbol_container_imp_t::find_by_vma(bfd_vma vma) const
{
	symbol_entry value;

	value.sample.vma = vma;

	vector<symbol_entry>::const_iterator it =
		lower_bound(symbols.begin(), symbols.end(),
			    value, less_sample_entry_by_vma());

	if (it != symbols.end() && it->sample.vma == vma)
		return &(*it);

	return 0;
}

void symbol_container_imp_t::get_symbols_by_count(size_t counter,
	profile_container_t::symbol_collection & v) const
{
	for (symbol_index_t i = 0 ; i < symbols.size() ; ++i)
		v.push_back(&symbols[i]);

	less_symbol_entry_by_samples_nr compare(counter);

	stable_sort(v.begin(), v.end(), compare);
}
