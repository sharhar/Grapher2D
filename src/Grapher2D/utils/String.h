#pragma once

#ifndef MY_STRING_H
#define MY_STRING_H

#include <iostream>
#include <string>
#include <vector>
#include <array>

using namespace std;

class String {
public:
	int length;
	char * buff;

	String() : length(0), buff(NULL) {

	}

	String(char init_val) :
		length(1),
		buff(new char[1]) {
		buff[0] = init_val;
	}

	String(char* init_val) :
		length(cStrlen(init_val)),
		buff(new char[length]) {
		cStrncpy(buff, init_val, length);
	}

	String(const String& other) :
		length(other.length),
		buff(new char[length]) {
		cStrncpy(buff, other.buff, length);
	}

	~String() {
		delete[] buff;
	}

	static int cStrlen(char * ptr) {
		int len = 0;
		char * p = ptr;
		while (p != NULL && *p != '\0') {
			len++;
			p++;
		}
		return len;
	}

	static void cStrncpy(char * ptr_dest, char * ptr_src, int n) {
		for (int i = 0; i < n; i++) {
			ptr_dest[i] = ptr_src[i];
		}
	}

	static void cStrncpy(char * ptr_dest, int off, char * ptr_src, int off2, int n) {
		for (int i = 0; i < n; i++) {
			ptr_dest[i + off] = ptr_src[i + off2];
		}
	}

	int size() { return length; }

	String substring(int start, int end) {
		String res;
		if (end == -1) {
			res.length = length - start;
			res.buff = new char[res.length];
			cStrncpy(res.buff, 0, buff, start, res.length);
			return res;
		}
		res.length = end - start;
		res.buff = new char[res.length];
		cStrncpy(res.buff, 0, buff, start, res.length);
		return res;
	}

	bool equals(String other) {
		if (length != other.length) {
			return false;
		}

		for (int i = 0; i < length; i++) {
			if (buff[i] != other.buff[i]) {
				return false;
			}
		}

		return true;
	}

	int find(String regex) {
		if (regex.length > length) {
			return -1;
		}

		for (int i = 0; i < length + 1 - regex.length; i++) {
			String part = substring(i, regex.length + i);
			if (part.equals(regex)) {
				return i;
			}
		}

		return -1;
	}

	std::vector<int> findAll(String regex) {
		std::vector<int> locs;
		int loc = find(regex);
		int off = 0;

		while (loc != -1) {
			locs.push_back(loc);
			off += loc + regex.length;
			loc = substring(loc + regex.length, -1).find(regex);
			if(loc != -1) {
				loc += off;
			}
		}

		return locs;
	}

	std::vector<String> split(String regex) {

		std::vector<int> locs = findAll(regex);
		std::vector<String> result;
		int last = 0;

		for (int i = 0; i < locs.size();i++) {
			String part = substring(last, locs[i]);
			last = locs[i] + regex.length;
			result.push_back(part);
		}

		String part = substring(last, -1);
		result.push_back(part);

		return result;
	}

	std::string getstdstring() {
		std::string result;
		for (int i = 0; i < length;i++) {
			result.push_back((*this)[i]);
		}

		return result;
	}

	String& operator= (const String& other) {
		if (this != &other) {
			delete[] buff;
			length = other.length; 
			buff = new char[length];
			cStrncpy(buff, other.buff, length);
		}
		return *this;
	}

	bool operator== (const String& other) {
		if (length != other.length) {
			return false;
		}

		for (int i = 0; i < length; i++) {
			if (buff[i] != other.buff[i]) {
				return false;
			}
		}

		return true;
	}

	char& operator[] (int index) {
		if (index < 0 || index > length) {
			cerr << "Invalid index in myString::operator[]. Aborting...\n";
			exit(-1);
		}
		return buff[index];
	}
};
namespace {
	String operator+(const String& s1, const String& s2) {
		String res;
		res.length = s1.length + s2.length;
		res.buff = new char[res.length];
		String::cStrncpy(res.buff, s1.buff, s1.length);
		String::cStrncpy(res.buff + s1.length, s2.buff, s2.length);
		return res;
	}

	String operator+(const String& s, char c) {
		String res;
		res.length = s.length + 1;
		res.buff = new char[res.length];
		String::cStrncpy(res.buff, s.buff, s.length);
		res.buff[res.length - 1] = c;
		return res;
	}

	String operator+(char c, const String& s) {
		String res;
		res.length = s.length + 1;
		res.buff = new char[res.length];
		res.buff[0] = c;
		String::cStrncpy(res.buff + 1, s.buff, s.length);
		return res;
	}

	String operator+(const String& s, char* c) {
		String res;
		int cLen = String::cStrlen(c);
		res.length = s.length + cLen;
		res.buff = new char[res.length];
		String::cStrncpy(res.buff, s.buff, s.length);
		String::cStrncpy(res.buff + s.length, c, cLen);
		return res;
	}

	String operator+(char* c, const String& s) {
		int cLen = String::cStrlen(c);
		String res;
		res.length = s.length + cLen;
		res.buff = new char[res.length];
		String::cStrncpy(res.buff, c, cLen);
		String::cStrncpy(res.buff + cLen, s.buff, s.length);
		return res;
	}

	ostream& operator<< (ostream& os, const String& s) {
		for (int i = 0; i < s.length; i++)  os.put(s.buff[i]);
		return os;
	}
}

#endif