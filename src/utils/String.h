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
	int len;
	char* buff;

	String();
	String(char init_val);
	String(char* init_val);
	String(std::string init_val);
	String(const String& other);
	~String();

	static int cStrlen(char * ptr);
	static void cStrncpy(char * ptr_dest, char * ptr_src, int n);
	static void cStrncpy(char * ptr_dest, int off, char * ptr_src, int off2, int n);

	int size();

	String substring(int start, int end);
	bool equals(String other);
	int find(String regex);
	void insert(String regex, int pos);
	std::vector<int> findAll(String regex);
	std::vector<String> split(String regex);
	std::string getstdstring();

	String& operator= (const String& other) {
		if (this != &other) {
			delete[] buff;
			len = other.len;
			buff = new char[len];
			cStrncpy(buff, other.buff, len);
		}
		return *this;
	}

	bool operator== (const String& other) {
		if (len != other.len) {
			return false;
		}

		for (int i = 0; i < len; i++) {
			if (buff[i] != other.buff[i]) {
				return false;
			}
		}

		return true;
	}

	char& operator[] (int index) {
		if (index < 0 || index > len) {
			cerr << "Invalid index in myString::operator[]. Aborting...\n";
			exit(-1);
		}
		return buff[index];
	}
};


namespace {
	String operator+(const String& s1, const String& s2) {
		String res;
		res.len = s1.len + s2.len;
		res.buff = new char[res.len];
		String::cStrncpy(res.buff, s1.buff, s1.len);
		String::cStrncpy(res.buff + s1.len, s2.buff, s2.len);
		return res;
	}

	String operator+(const String& s, char c) {
		String res;
		res.len = s.len + 1;
		res.buff = new char[res.len];
		String::cStrncpy(res.buff, s.buff, s.len);
		res.buff[res.len - 1] = c;
		return res;
	}

	String operator+(char c, const String& s) {
		String res;
		res.len = s.len + 1;
		res.buff = new char[res.len];
		res.buff[0] = c;
		String::cStrncpy(res.buff + 1, s.buff, s.len);
		return res;
	}

	String operator+(const String& s, char* c) {
		String res;
		int cLen = String::cStrlen(c);
		res.len = s.len + cLen;
		res.buff = new char[res.len];
		String::cStrncpy(res.buff, s.buff, s.len);
		String::cStrncpy(res.buff + s.len, c, cLen);
		return res;
	}

	String operator+(char* c, const String& s) {
		int cLen = String::cStrlen(c);
		String res;
		res.len = s.len + cLen;
		res.buff = new char[res.len];
		String::cStrncpy(res.buff, c, cLen);
		String::cStrncpy(res.buff + cLen, s.buff, s.len);
		return res;
	}

	ostream& operator<< (ostream& os, const String& s) {
		for (int i = 0; i < s.len; i++)  os.put(s.buff[i]);
		return os;
	}
}

#endif