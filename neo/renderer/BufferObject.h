/*
===========================================================================

Doom 3 BFG Edition GPL Source Code
Copyright (C) 1993-2012 id Software LLC, a ZeniMax Media company. 

This file is part of the Doom 3 BFG Edition GPL Source Code ("Doom 3 BFG Edition Source Code").  

Doom 3 BFG Edition Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 BFG Edition Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 BFG Edition Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 BFG Edition Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 BFG Edition Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/
#ifndef __BUFFEROBJECT_H__
#define __BUFFEROBJECT_H__

/*
================================================================================================

	Buffer Objects

================================================================================================
*/

enum bufferMapType_t {
	BM_READ,			// map for reading
	BM_WRITE			// map for writing
};

// Returns all targets to virtual memory use instead of buffer object use.
// Call this before doing any conventional buffer reads, like screenshots.
void UnbindBufferObjects();

/*
================================================
idBufferObject
================================================
*/
class idBufferObject {
public:
						idBufferObject();

	bool				IsMapped() const { return ( size & MAPPED_FLAG ) != 0; }

	int					GetSize() const { return ( size & ~MAPPED_FLAG ); }
	int					GetAllocedSize() const { return ( ( size & ~MAPPED_FLAG ) + 15 ) & ~15; }
	unsigned int		GetAPIObject() const { return apiObject; }
	int					GetOffset() const { return ( offsetInOtherBuffer & ~OWNS_BUFFER_FLAG ); }

protected:
	int					size;					// size in bytes
	int					offsetInOtherBuffer;	// offset in bytes
	unsigned int		apiObject;

	// sizeof() confuses typeinfo...
	static const int	MAPPED_FLAG			= 1 << ( 4 /* sizeof( int ) */ * 8 - 1 );
	static const int	OWNS_BUFFER_FLAG	= 1 << ( 4 /* sizeof( int ) */ * 8 - 1 );
	
	void				ClearWithoutFreeing();
	void				SetMapped() const { const_cast< int & >( size ) |= MAPPED_FLAG; }
	void				SetUnmapped() const { const_cast< int & >( size ) &= ~MAPPED_FLAG; }
	bool				OwnsBuffer() const { return ( ( offsetInOtherBuffer & OWNS_BUFFER_FLAG ) != 0 ); }
};

/*
================================================
idVertexBuffer
================================================
*/
class idVertexBuffer : public idBufferObject {
public:
						idVertexBuffer() : idBufferObject() {};
						~idVertexBuffer();

	// Allocate or free the buffer.
	bool				AllocBufferObject( const void * data, int allocSize );
	void				FreeBufferObject();

	// Make this buffer a reference to another buffer.
	void				Reference( const idVertexBuffer & other );
	void				Reference( const idVertexBuffer & other, int refOffset, int refSize );

	// Copies data to the buffer. 'size' may be less than the originally allocated size.
	void				Update( const void * data, int updateSize ) const;

	void *				MapBuffer( bufferMapType_t mapType ) const;
	void				UnmapBuffer() const;

	DISALLOW_COPY_AND_ASSIGN( idVertexBuffer );
};

/*
================================================
idIndexBuffer
================================================
*/
class idIndexBuffer : public idBufferObject {
public:
						idIndexBuffer() : idBufferObject() {};
						~idIndexBuffer();

	// Allocate or free the buffer.
	bool				AllocBufferObject( const void * data, int allocSize );
	void				FreeBufferObject();

	// Make this buffer a reference to another buffer.
	void				Reference( const idIndexBuffer & other );
	void				Reference( const idIndexBuffer & other, int refOffset, int refSize );

	// Copies data to the buffer. 'size' may be less than the originally allocated size.
	void				Update( const void * data, int updateSize ) const;

	void *				MapBuffer( bufferMapType_t mapType ) const;
	void				UnmapBuffer() const;

	DISALLOW_COPY_AND_ASSIGN( idIndexBuffer );
};

/*
================================================
idUniformBuffer

IMPORTANT NOTICE: on the PC, binding to an offset in uniform buffer objects 
is limited to GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, which is 256 on current nvidia cards,
so joint offsets, which are multiples of 48 bytes, must be in multiples of 16 = 768 bytes.
================================================
*/
class idUniformBuffer : public idBufferObject {
public:
						idUniformBuffer() : idBufferObject() {};
						~idUniformBuffer();

	// Allocate or free the buffer.
	bool				AllocBufferObject( const void * data, int allocSize );
	void				FreeBufferObject();

	// Make this buffer a reference to another buffer.
	void				Reference( const idUniformBuffer & other );
	void				Reference( const idUniformBuffer & other, int refOffset, int refSize );

	// Copies data to the buffer. 'size' may be less than the originally allocated size.
	void				Update( const void * data, int updateSize ) const;

	void *				MapBuffer( bufferMapType_t mapType ) const;
	void				UnmapBuffer() const;

	DISALLOW_COPY_AND_ASSIGN( idUniformBuffer );
};

#endif // !__BUFFEROBJECT_H__
