#pragma once
#include <string.h>

inline char* getFileDirectory(char* path) {
	if (path == 0) {
		return path;
	}
	char* lastDelim = strrchr(path, '\\');
	if (lastDelim == 0) {
		lastDelim = strrchr(path, '/');
	}
	if (lastDelim == 0) {
		return 0;
	}
	lastDelim[0] = '\0';
	return path;
}

inline char* getFileName(char* path) {
	if (path == 0) {
		return path;
	}
	char* lastDelim = strrchr(path, '\\');
	if (lastDelim == 0) {
		lastDelim = strrchr(path, '/');
	}
	if (lastDelim == 0) {
		return 0;
	}
	lastDelim++;
	memmove((void*)path, lastDelim, lastDelim - path);
	return path;
}