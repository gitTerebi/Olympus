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
#ifndef PKWAREINPUTSTREAM_H
#define PKWAREINPUTSTREAM_H

#include <cstdint>
#include <string>

class QFile;

/**
* Internal class
*/
class PKDictionary;

/**
* Input class for reading files / blocks of data compressed with the
* PKWare Compression Library.
* All methods (including constructors) may throw a PKException
*/
class PKWareInputStream {
	public:
		/**
		* Constructor
		* @param stream Open input stream to read from
		* @param close_stream Whether to close the stream upon destroying
		* this object. Set to FALSE if you want to continue reading from
		* the stream after decompressing
		* @param file_length Length of the compressed data. If not given, the
		* object tries to figure it out, but may read beyond the end of the
		* compressed block
		*/
        PKWareInputStream(QFile *in, int file_length,
			bool close_stream = true);
		~PKWareInputStream();
		
		/**
		* Reads a single byte from the compressed stream
		*/
		uint8_t read();
		
		/**
		* Reads a block of data of maximum length `length' from the stream
		* @param buf Place to put read data
		* @param length Maximum length to read
		* @return int Number of bytes actually read, may be less than
		* `length' if and only if EOF is encountered
		*/
		int read(uint8_t *buf, int length);
		
		/**
		* Reads a byte from the input stream. Same as read()
		*/
		uint8_t readByte();
		
		/**
		* Reads a little-endian short (2 bytes) from the input stream.
		*/
		uint16_t readShort();
		
		/**
		* Reads a little-endian int (4 bytes) from the input stream.
		*/
		uint32_t readInt();
		
		/**
		* Skips over `length' bytes.
		*/
		void skip(int length);
		
		/**
		* Empties the stream, reading until EOF is encountered
		*/
		void empty();
		
		/**
		* Returns whether there was an error during decompression
		*/
		bool hasError();
		
		/**
		* Returns the last error message. Behaviour is undefined if
		* {@link hasError()} returns false
		*/
        std::string error();
		
		/**
		* Returns whether end-of-file is reached
		*/
		bool atEnd();
		
	private:
		void init();
		void readHeader();
		int getCopyLength();
		int getCopyOffset(int length);
		int getCopyOffsetHigh();
		int skipBytes(int length);
		int reverse(int number, int length);
		void fillBuffer();
		void advanceByte();
		unsigned char readBit();
		int readBits(int length);
        void setError(const std::string &error);
		
		// Class variables (comments is where they're initialised)
        QFile *input; // ctor
		int dictSize; // readHeader
		char *buffer; // fillBuffer
		int bufOffset; // fillBuffer
		int bufBit; // init
		int dictionary_bits; // readHeader
		PKDictionary *dictionary; // readHeader
		
		// For the reading of bytes:
		int read_offset; // init
		int read_length; // init
		bool read_copying; // init
		int file_length; // ctor or init
		
		// For detecting end of stream:
		bool eof_reached; // init, fillBuffer
		int eof_position; // fillBuffer
		bool close_stream; // ctor
		
		bool at_end;
		bool has_error;
        std::string errorMessage;
		
		static const int BUFFER_SIZE = 4096;
};

#endif /* PKWAREINPUTSTREAM_H */
