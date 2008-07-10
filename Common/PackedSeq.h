#ifndef PACKEDSEQ_H
#define PACKEDSEQ_H

//#define SAVE_MEM
#include <list>
#include "CommonUtils.h"
#include "SeqExt.h"

enum SeqFlag
{
	SF_SEEN = 0x1,
	SF_DELETE = 0x2
};


//
// Sense extension is index 0, antisense extension is index 1
//
struct ExtensionRecord
{
	SeqExt dir[2];	
};

class PackedSeq
{
	public:
		PackedSeq();
		PackedSeq(const Sequence& seq);
		
		// Write this packed sequence to the buffer
		size_t serialize(char* buffer) const;
		
		// Read this packed sequence from the buffer
		size_t unserialize(const char* buffer);		
		
		// Assignment Operator
		PackedSeq& operator=(const PackedSeq& other);
		
		// Operators
		inline bool operator==(const PackedSeq& other) const
		{
			return compare(other) == 0;
		}
		
		inline bool operator!=(const PackedSeq& other) const
		{
			return compare(other) != 0;
		}
		inline bool operator<(const PackedSeq& other) const
		{
			return compare(other) < 0;	
		}
		
		// Comparison
		int compare(const PackedSeq& other) const;
		
		// Decode the sequence
		Sequence decode() const;
		Sequence decodeByte(const char byte) const;
		
		unsigned getCode() const;
		size_t getHashCode() const;
		
		// get a subsequence of this packed seq
		PackedSeq subseq(unsigned start, unsigned len) const;		
		
		// get the length of the sequence
		unsigned getSequenceLength() const;
		
		// get the multiplicity of this sequence
		unsigned getMultiplicity() const { return m_multiplicity; }
		
		// add to the multiplicity
		void addMultiplicity()
		{
			++m_multiplicity;
			assert(m_multiplicity > 0);
		}
		
		// Return the pointer to the data
		const char* const getDataPtr() const;
		
		// get a particular base
		char getFirstBase() const { return getBaseChar(0); }
		char getLastBase() const { return getBaseChar(m_length - 1); }
		uint8_t getBaseCode(unsigned seqIndex) const;
		char getBaseChar(unsigned seqIndex) const;
		
		// flags
		void setFlag(SeqFlag flag) { m_flags |= flag; }
		bool isFlagSet(SeqFlag flag) const { return m_flags & flag; }
		void clearFlag(SeqFlag flag) { m_flags &= ~flag; }
		
		// Extension management
		SeqExt getExtension(extDirection dir) const;
		void setExtension(extDirection dir, SeqExt extension);
		void setBaseExtension(extDirection dir, char b);
		void clearExtension(extDirection dir, char b);
		void clearAllExtensions(extDirection dir);
		bool checkExtension(extDirection dir, char b) const;
		bool hasExtension(extDirection dir) const;
		bool isAmbiguous(extDirection dir) const;
		void printExtension() const;
		
		// Reverse and complement this sequence
		void reverseComplement();
		
		// append/prepend
		// these functions preserve the length of the sequence by shifting first before adding the new base
		// the base shifted off is returned
		char rotate(extDirection dir, char base);
		char shiftAppend(char base);
		char shiftPrepend(char base);
		void setLastBase(extDirection dir, char base);
		
		// Print
		void print() const;
		
		// The maximum kmer size is hardcoded to be 40
		// Why is this? If we use a dynamically allocated character buffer malloc/new will give us 16 or 32 bytes no matter how much we want
		// This padding + the size of the pointer effectively negates the gains from use a compressed sequence
		// By hardcoding this value we can keep things aligned, plus remove the need for alloc/frees
		// The alternatives are a) accepting the inefficiency of small dynamic allocations or b) writing a custom small object allocator
		static const unsigned MAX_KMER = 40;
		static const unsigned NUM_BYTES = MAX_KMER / 4;

	private:
		// get/set a particular value
		static inline void setBaseCode(char* pSeq,
				unsigned seqIndex, uint8_t base);
		static inline void setBaseCode(char* pSeq,
				unsigned byteNum, unsigned index, uint8_t base);
		static inline void setBaseChar(char* pSeq,
				unsigned seqIndex, char base);
		static inline void setBaseChar(char* pSeq,
				unsigned byteNum, unsigned index, char base);
		static inline uint8_t getBaseCode(const char* pSeq,
				unsigned byteNum, unsigned index);
		static inline char getBaseChar(const char* pSeq,
				unsigned byteNum, unsigned index);
		
		// Create the two bit code for the base
		static inline uint8_t baseToCode(char base);
		static inline char codeToBase(uint8_t code);
		
		// Get the number of bytes in the sequence
		static inline unsigned getNumCodingBytes(unsigned seqLength);
		static inline unsigned seqIndexToByteNumber(unsigned seqIndex);
		static inline unsigned seqIndexToBaseIndex(unsigned seqIndex);
		
		// shift a single byte
		static char leftShiftByte(char* pSeq,
				unsigned byteNum, unsigned index, char base);
		static char rightShiftByte(char* pSeq,
				unsigned byteNum, unsigned index, char base);
		
		char m_seq[NUM_BYTES];
		uint8_t m_length;
		char m_flags;
		uint16_t m_multiplicity;
		ExtensionRecord m_extRecord;
};

// Global function to make a reverse complement of a packed seq
PackedSeq reverseComplement(const PackedSeq& seq);


#endif
