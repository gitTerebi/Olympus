/*
 *   Citybuilding Mappers - create minimaps from citybuilding game files
 *   Copyright (C) 2007, 2008  Bianca van Schaik
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*
This class is a C++ port from the Dynamite library, written in C.
http://synce.sourceforge.net/synce/dynamite.php
Dyamite library licence:

Copyright (c) 2003 David Eriksson <twogood@users.sourceforge.net>

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "pkwareinputstream.h"

#include "qfile.h"

// #include <QDebug>

/**
* Dictionary for keeping track of past read bytes
*/
class PKDictionary {
	public:
		/**
		* Creates a new dictionary of size `size'
		*/
		PKDictionary(int size) {
			dictionary = new uint8_t[size];
			this->size = size;
			this->first = -1;
		}
		
		~PKDictionary() {
			delete[] dictionary;
		}
		
		/**
		* Returns the byte at the specified position.
		* Also does a PUT for this byte since the compression
		* algorithm requires it
		*/
		uint8_t get(int position) {
			int index = (size + first - position) % size;
			put(dictionary[index]);
			return dictionary[index];
		}
		
		/**
		* Adds a byte to the dictionary
		*/
		void put(uint8_t b) {
			first = (first + 1) % size;
			dictionary[first] = b;
		}
	
	private:
		int size;
		int first;
		uint8_t *dictionary;
};

PKWareInputStream::PKWareInputStream(QFile* in, int file_length, bool close_stream) {
	input = in;
	this->close_stream = close_stream;
	this->file_length = file_length;
	init();
}

PKWareInputStream::~PKWareInputStream() {
	if (dictionary) delete dictionary;
	if (buffer) delete[] buffer;
	if (close_stream) {
		delete input;
	}
}

uint8_t PKWareInputStream::read() {
	if (has_error) return 0;
	
	if (read_copying) {
		read_length--;
		if (read_length <= 0) {
			read_copying = false;
		}
		return (uint8_t)dictionary->get(read_offset);
	} else {
		if (readBit() == 0) {
			// Copy byte verbatim
			int result = readBits(8);
			dictionary->put((uint8_t)result);
			return (uint8_t)result;
		}
		// Needs to copy stuff from the dictionary
		read_length = getCopyLength();
		if (read_length >= 519) {
			has_error = at_end = true;
			return 0;
		}
		
		read_offset = getCopyOffset(read_length);
		read_length--;
		read_copying = true;
		return dictionary->get(read_offset);
	}
}

int PKWareInputStream::read(uint8_t *buf, int length) {
	if (has_error) return 0;
	if (buf == NULL) {
		return skipBytes(length);
	}
	
	int current = 0;
	while (current < length && !has_error) {
		if (read_copying) {
			while (current < length && read_length > 0) {
				read_length--;
				buf[current++] = dictionary->get(read_offset);
			}
			if (read_length <= 0) {
				read_copying = false;
			}
		} else if (readBit() == 0) {
			// Copy byte verbatim
			int result = readBits(8);
			dictionary->put((uint8_t)result);
			buf[current++] = (uint8_t)result;
		} else {
			// Needs to copy stuff from the dictionary
			read_length = getCopyLength();
			if (read_length >= 519) {
				has_error = at_end = true;
				return current;
			}
			
			read_offset = getCopyOffset(read_length);
			buf[current++] = dictionary->get(read_offset);
			read_length--;
			read_copying = true;
		}
	}
	return current;
}

uint8_t PKWareInputStream::readByte() {
	if (has_error) return 0;
	return read();
}

uint16_t PKWareInputStream::readShort() {
	if (has_error) return 0;
	uint8_t data[2];
	read(data, 2);
	return (uint16_t)(data[0] + (data[1] << 8));
}

uint32_t PKWareInputStream::readInt() {
	if (has_error) return 0;
	uint8_t data[4];
	uint32_t number = 0;
	
	read(data, 4);
	for (int i = 0; i < 4; i++) {
		number += (data[i] << (i*8));
	}
	return number;
}

/**
* Skips length bytes from the input.
*/
void PKWareInputStream::skip(int length) {
	skipBytes(length);
}

int PKWareInputStream::skipBytes(int length) {
	if (has_error) return 0;
	
	int current = 0;
	while (current < length && !has_error) {
		if (read_copying) {
			while (current < length && read_length > 0) {
				read_length--;
				dictionary->get(read_offset);
				current++;
			}
			if (read_length <= 0) {
				read_copying = false;
			}
		} else if (readBit() == 0) {
			// Copy byte verbatim
			dictionary->put((uint8_t) readBits(8));
			current++;
		} else {
			// Needs to copy stuff from the dictionary
			read_length = getCopyLength();
			if (read_length >= 519) {
				has_error = at_end = true;
				return current;
			}
			
			read_offset = getCopyOffset(read_length);
			dictionary->get(read_offset);
			current++;
			read_length--;
			read_copying = true;
		}
	}
	return current;
}

/**
* Reads from the stream (and discards) until EOF is encountered
*/
void PKWareInputStream::empty() {
	if (has_error) return;
	
	while (!has_error) {
		read();
	}
}

bool PKWareInputStream::atEnd() {
	return at_end;
}

bool PKWareInputStream::hasError() {
	return has_error && !at_end;
}

std::string PKWareInputStream::error() {
	return errorMessage;
}

/**
* Initialises the stream
*/
void PKWareInputStream::init() {
	// Init the variables
	has_error = false;
	at_end = false;
	eof_reached = false;
	dictionary = NULL;
	buffer = NULL;
	
	bufBit = 0;
	read_offset = 0;
	read_length = 0;
	read_copying = false;
	
	if (file_length <= 2) {
		setError("File too small");
		return;
	}
	
	readHeader();
	if (has_error) {
		return;
	}
	fillBuffer();
}

/**
* Reads the 2-byte header and initialises the dictionary
*/
void PKWareInputStream::readHeader() {
	// Read the header to decide on the encoding type
	char c;
	input->read(&c, 1);
	if (c != 0) {
		setError("Static dictionary not supported");
		return;
	}
	
	input->read(&c, 1);
	dictionary_bits = (int)c;
	switch (dictionary_bits) {
		case 4: dictSize = 1024; break;
		case 5: dictSize = 2048; break;
		case 6: dictSize = 4096; break;
		default:
			setError("Unknown dictionary size");
			return;
	}
	dictionary = new PKDictionary(dictSize);
	buffer = new char[BUFFER_SIZE];
	file_length -= 2; // Subtract two header bytes from total file length
}

/**
* Gets the amount of bytes to copy from the dictionary
*/
int PKWareInputStream::getCopyLength() {
	int bits;
	
	bits = readBits(2);
	if (bits == 3) { // 11
		return 3;
	} else if (bits == 1) { // 10x
		return 4 - 2 * readBit();
	} else if (bits == 2) { // 01
		if (readBit() == 1) { // 011
			return 5;
		} else { // 010x
			return 7 - readBit();
		}
	} else if (bits == 0) { // 00
		bits = readBits(2);
		if (bits == 3) { // 0011
			return 8;
		} else if (bits == 1) { // 0010
			if (readBit() == 1) { // 00101
				return 9;
			} else { // 00100x
				return 10 + readBit();
			}
		} else if (bits == 2) { // 0001
			if (readBit() == 1) { // 00011xx
				return 12 + readBits(2);
			} else { // 00010xxx
				return 16 + readBits(3);
			}
		} else if (bits == 0) { // 0000
			bits = readBits(2);
			switch (bits) {
				case 3: return 24 + readBits(4); // 000011xxxx
				case 1: return 40 + readBits(5); // 000010xxxxx
				case 2: return 72 + readBits(6); // 000001xxxxxx
				case 0:
					if (readBit()) {
						return 136 + readBits(7); // 0000001xxxxxxx
					} else {
						return 264 + readBits(8); // 0000000xxxxxxxx
					}
			}
		}
	}
	// Cannot happen
	return -1;
}

/**
* Gets the offset at which to start copying bytes from the dictionary
*/
int PKWareInputStream::getCopyOffset(int length) {
	int lower_bits, result;
	if (length == 2) {
		lower_bits = 2;
	} else {
		lower_bits = dictionary_bits;
	}
	
	result = getCopyOffsetHigh() << lower_bits;
	result |= readBits(lower_bits);
	return result;
}

/**
* Gets the "high" value of the copy offset, the lower N bits
* are stored verbatim; N depends on the copy length and the
* dictionary size.
*/
int PKWareInputStream::getCopyOffsetHigh() {
	int bits;
	
	bits = readBits(2);
	if (bits == 3) { // 11
		return 0;
	} else if (bits == 1) { // 10
		bits = readBits(2);
		switch (bits) {
			case 0: return 0x6 - readBit(); // 1000x
			case 1: return 0x2; // 1010
			case 2: return 0x4 - readBit(); // 1001x
			case 3: return 0x1; // 1011
		}
	} else if (bits == 2) { // 01
		bits = readBits(4);
		if (bits == 0) {
			return 0x17 - readBit();
		} else {
			return 0x16 - reverse(bits, 4);
		}
	} else if (bits == 0) { // 00
		bits = readBits(2);
		switch (bits) {
			case 3: return 0x1f - reverse(readBits(3), 3);
			case 1: return 0x27 - reverse(readBits(3), 3);
			case 2: return 0x2f - reverse(readBits(3), 3);
			case 0: return 0x3f - reverse(readBits(4), 4);
		}
	}
	// Cannot happen
	return -1;
}

/**
* Reverse the bits in `number', essentially converting it from little
* endian to big endian or vice versa.
*/
int PKWareInputStream::reverse(int number, int length) {
	if (length == 3) {
		switch (number) {
			case 1: return 4;
			case 3: return 6;
			case 4: return 1;
			case 6: return 3;
			default: return number;
		}
	} else if (length == 4) {
		switch (number) {
			case 1: return 8;
			case 2: return 4;
			case 3: return 12;
			case 4: return 2;
			case 5: return 10;
			case 7: return 14;
			case 8: return 1;
			case 10: return 5;
			case 11: return 13;
			case 12: return 3;
			case 13: return 11;
			case 14: return 7;
			default: return number;
		}
	}
	return number;
}

/**
* Fill the internal buffer
*/
void PKWareInputStream::fillBuffer() {
	bufOffset = 0;
	if (file_length <= BUFFER_SIZE) {
		input->read(buffer, file_length);
		eof_reached = true;
		eof_position = file_length;
	} else {
		input->read(buffer, BUFFER_SIZE);
		file_length -= BUFFER_SIZE;
	}
}

/**
* Advances the data pointer one byte, filling the buffer
* if necessary
*/
void PKWareInputStream::advanceByte() {
	bufOffset++;
	if (eof_reached && bufOffset >= eof_position) {
		setError("Unexpected EOF");
		return;
	}
	if (bufOffset >= BUFFER_SIZE) {
		fillBuffer();
	}
	bufBit = 0;
}

/**
* Reads one single bit
*/
uint8_t PKWareInputStream::readBit() {
	if (bufBit == 8) {
		advanceByte();
	}
	uint8_t b = (uint8_t) (buffer[bufOffset] >> bufBit) & 1;
	bufBit++;
	return b;
}

/**
* Reads bits in little endian order
* @param length Number of bits to read. Should never be more than 8.
* @return int Value of the bits read
*/
int PKWareInputStream::readBits(int length) {
	int result;
	if (bufBit == 8) {
		advanceByte();
	}
	// Check to see if we span multiple bytes
	if (bufBit + length > 8) {
		// First take last remaining bits in this byte & put them in place
		// Do "& 0xff" to prevent a negative character from filling with ff's
		result = ((buffer[bufOffset] & 0xff) >> bufBit);
		int length1 = 8 - bufBit;
		int length2 = length - length1;
		advanceByte();
		
		// Read length2 bits from the second byte & add them to the result
		result |= ((buffer[bufOffset]) & ((1 << length2) - 1)) << length1;
		bufBit = length2;
	} else {
		// Same byte, easy!
		result = (buffer[bufOffset] >> bufBit) & ((1 << length) - 1);
		bufBit += length;
	}
	return result;
}

void PKWareInputStream::setError(const std::string& message) {
	//qDebug() << "Setting error to true: " << message;
	errorMessage = message;
	has_error = true;
}
