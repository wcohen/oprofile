/**
 * @file file_manip.cpp
 * Useful file management helpers
 *
 * @remark Copyright 2002 OProfile authors
 * @remark Read the file COPYING
 *
 * @author Philippe Elie <phil_el@wanadoo.fr>
 * @author John Levon <moz@compsoc.man.ac.uk>
 */

#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fnmatch.h>

#include <cstdio>
#include <cerrno>
#include <iostream>
#include <vector>

#include "op_file.h"

#include "file_manip.h"
#include "string_manip.h"

using std::vector;
using std::string;
using std::list;
using std::cerr;
using std::endl;

/**
 * is_file_identical - check for identical files
 * @param file1  first filename
 * @param file2  scond filename
 *
 * return true if the two filenames belong to the same file
 */
bool is_files_identical(string const & file1, string const & file2)
{
	struct stat st1;
	struct stat st2;

	if (stat(file1.c_str(), &st1) == 0 && stat(file2.c_str(), &st2) == 0) {
		if (st1.st_dev == st2.st_dev && st1.st_ino == st2.st_ino)
			return true;
	}

	return false;
}

/**
 * create_dir - create a directory
 * @param dir  the directory name to create
 *
 * return false if the directory @dir does not exist
 * and cannot be created
 */
bool create_dir(string const & dir)
{
	if (access(dir.c_str(), F_OK)) {
		if (mkdir(dir.c_str(), 0700))
			return false;
	}
	return true;
}

/**
 * create_path - create a path
 * @param path  the path to create
 *
 * create directory for each dir components in @path
 * return false if one of the path cannot be created.
 */
bool create_path(string const & path)
{
	vector<string> path_component;

	size_t slash = 0;
	while (slash < path.length()) {
		size_t new_pos = path.find_first_of('/', slash);
		if (new_pos == string::npos)
			new_pos = path.length();

		path_component.push_back(path.substr(slash, (new_pos - slash) + 1));
		slash = new_pos + 1;
	}

	string dir_name;
	for (size_t i = 0 ; i < path_component.size() ; ++i) {
		dir_name += '/' + path_component[i];
		if (!create_dir(dir_name))
			return false;
	}
	return true;
}

/**
 * op_read_link - read the contents of a symbolic link file
 * @param name  the file name
 *
 * return an empty string on failure
 */
string op_read_link(string const & name)
{
	char linkbuf[FILENAME_MAX];
	int c;

	c = readlink(name.c_str(), linkbuf, FILENAME_MAX);

	if (c == -1)
		return string();

	if (c == FILENAME_MAX)
		linkbuf[FILENAME_MAX-1] = '\0';
	else
		linkbuf[c] = '\0';
	return linkbuf;
}


bool op_file_readable(string const & file)
{
	return op_file_readable(file.c_str());
}

inline static bool is_directory_name(char const * name)
{
	return name[0] == '.' &&
		(name[1] == '\0' ||
		 (name[1] == '.' && name[2] == '\0'));
}


bool create_file_list(list<string> & file_list, string const & base_dir,
		      string const & filter, bool recursive)
{
	DIR *dir;
	struct dirent * ent;

	if (!(dir = opendir(base_dir.c_str())))
		return false;

	while ((ent = readdir(dir)) != 0) {
		if (!is_directory_name(ent->d_name) &&
		    fnmatch(filter.c_str(), ent->d_name, 0) != FNM_NOMATCH) {
			if (recursive) {
				struct stat stat_buffer;
				string name = base_dir + '/' + ent->d_name;
				if (stat(name.c_str(), &stat_buffer) == 0) {
					if (S_ISDIR(stat_buffer.st_mode) &&
					    !S_ISLNK(stat_buffer.st_mode)) {
						// recursive retrieve
						create_file_list(file_list,
								 name, filter,
								 recursive);
					} else {
						file_list.push_back(name);
					}
				}
			}
			else {
				file_list.push_back(ent->d_name);
			}
		}
	}

	closedir(dir);

	return true;
}


std::string relative_to_absolute_path(string const & path,
				      string const & base_dir)
{
	char const * dir = 0;

	// don't screw up on already absolute paths
	if ((path.empty() || path[0] != '/') && !base_dir.empty())
		dir = base_dir.c_str();

	char * result = op_relative_to_absolute_path(path.c_str(), dir);

	string res(result);

	free(result);

	return res;
}

/**
 * dirname - get the path component of a filename
 * @param file_name  filename
 *
 * Returns the path name of a filename with trailing '/' removed.
 */
string dirname(string const & file_name)
{
	return erase_from_last_of(file_name, '/');
}

/**
 * basename - get the basename of a path
 * @param path_name  path
 *
 * Returns the basename of a path with trailing '/' removed.
 */
string basename(string const & path_name)
{
	string result = rtrim(path_name, '/');

	return erase_to_last_of(result, '/');
}