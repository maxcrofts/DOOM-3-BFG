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
#pragma hdrstop
#include "../idlib/precompiled.h"
#include "tr_local.h"

idCVar r_showBuffers( "r_showBuffers", "0", CVAR_INTEGER, "" );


static const GLenum bufferUsage = GL_DYNAMIC_DRAW;

/*
==================
IsWriteCombined
==================
*/
bool IsWriteCombined( void * base ) {
#ifdef ID_WIN
	MEMORY_BASIC_INFORMATION info;
	SIZE_T size = VirtualQueryEx( GetCurrentProcess(), base, &info, sizeof( info ) );
	if ( size == 0 ) {
		DWORD error = GetLastError();
		error = error;
		return false;
	}
	bool isWriteCombined = ( ( info.AllocationProtect & PAGE_WRITECOMBINE ) != 0 );
	return isWriteCombined;
#else
	return true;
#endif
}



/*
================================================================================================

	Buffer Objects

================================================================================================
*/

/*
========================
UnbindBufferObjects
========================
*/
void UnbindBufferObjects() {
	qglBindBufferARB( GL_ARRAY_BUFFER, 0 );
	qglBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

#ifdef ID_X86_SSE2_INTRIN

void CopyBuffer( byte * dst, const byte * src, int numBytes ) {
	assert_16_byte_aligned( dst );
	assert_16_byte_aligned( src );

	int i = 0;
	for ( ; i + 128 <= numBytes; i += 128 ) {
		__m128i d0 = _mm_load_si128( (__m128i *)&src[i + 0*16] );
		__m128i d1 = _mm_load_si128( (__m128i *)&src[i + 1*16] );
		__m128i d2 = _mm_load_si128( (__m128i *)&src[i + 2*16] );
		__m128i d3 = _mm_load_si128( (__m128i *)&src[i + 3*16] );
		__m128i d4 = _mm_load_si128( (__m128i *)&src[i + 4*16] );
		__m128i d5 = _mm_load_si128( (__m128i *)&src[i + 5*16] );
		__m128i d6 = _mm_load_si128( (__m128i *)&src[i + 6*16] );
		__m128i d7 = _mm_load_si128( (__m128i *)&src[i + 7*16] );
		_mm_stream_si128( (__m128i *)&dst[i + 0*16], d0 );
		_mm_stream_si128( (__m128i *)&dst[i + 1*16], d1 );
		_mm_stream_si128( (__m128i *)&dst[i + 2*16], d2 );
		_mm_stream_si128( (__m128i *)&dst[i + 3*16], d3 );
		_mm_stream_si128( (__m128i *)&dst[i + 4*16], d4 );
		_mm_stream_si128( (__m128i *)&dst[i + 5*16], d5 );
		_mm_stream_si128( (__m128i *)&dst[i + 6*16], d6 );
		_mm_stream_si128( (__m128i *)&dst[i + 7*16], d7 );
	}
	for ( ; i + 16 <= numBytes; i += 16 ) {
		__m128i d = _mm_load_si128( (__m128i *)&src[i] );
		_mm_stream_si128( (__m128i *)&dst[i], d );
	}
	for ( ; i + 4 <= numBytes; i += 4 ) {
		*(uint32 *)&dst[i] = *(const uint32 *)&src[i];
	}
	for ( ; i < numBytes; i++ ) {
		dst[i] = src[i];
	}
	_mm_sfence();
}

#else

void CopyBuffer( byte * dst, const byte * src, int numBytes ) {
	assert_16_byte_aligned( dst );
	assert_16_byte_aligned( src );
	memcpy( dst, src, numBytes );
}

#endif

/*
================================================================================================

	idBufferObject

================================================================================================
*/

/*
========================
idBufferObject::idBufferObject
========================
*/
idBufferObject::idBufferObject() {
	size = 0;
	offsetInOtherBuffer = OWNS_BUFFER_FLAG;
	apiObject = 0;
	SetUnmapped();
}

/*
========================
idBufferObject::ClearWithoutFreeing
========================
*/
void idBufferObject::ClearWithoutFreeing() {
	size = 0;
	offsetInOtherBuffer = OWNS_BUFFER_FLAG;
	apiObject = 0;
}


/*
================================================================================================

	idVertexBuffer

================================================================================================
*/

/*
========================
idVertexBuffer::~idVertexBuffer
========================
*/
idVertexBuffer::~idVertexBuffer() {
	FreeBufferObject();
}

/*
========================
idVertexBuffer::AllocBufferObject
========================
*/
bool idVertexBuffer::AllocBufferObject( const void * data, int allocSize ) {
	assert( apiObject == NULL );
	assert_16_byte_aligned( data );

	if ( allocSize <= 0 ) {
		idLib::Error( "idVertexBuffer::AllocBufferObject: allocSize = %i", allocSize );
	}

	size = allocSize;

	bool allocationFailed = false;

	int numBytes = GetAllocedSize();


	// clear out any previous error
	qglGetError();

	GLuint bufferObject = 0xFFFF;
	qglGenBuffersARB( 1, & bufferObject );
	if ( bufferObject == 0xFFFF ) {
		idLib::FatalError( "idVertexBuffer::AllocBufferObject: failed" );
	}
	qglBindBufferARB( GL_ARRAY_BUFFER, bufferObject );

	// these are rewritten every frame
	qglBufferDataARB( GL_ARRAY_BUFFER, numBytes, NULL, bufferUsage );
	apiObject = bufferObject;

	GLenum err = qglGetError();
	if ( err == GL_OUT_OF_MEMORY ) {
		idLib::Warning( "idVertexBuffer::AllocBufferObject: allocation failed" );
		allocationFailed = true;
	}


	if ( r_showBuffers.GetBool() ) {
		idLib::Printf( "vertex buffer alloc %p, api %p (%i bytes)\n", this, GetAPIObject(), GetSize() );
	}

	// copy the data
	if ( data != NULL ) {
		Update( data, allocSize );
	}

	return !allocationFailed;
}

/*
========================
idVertexBuffer::FreeBufferObject
========================
*/
void idVertexBuffer::FreeBufferObject() {
	if ( IsMapped() ) {
		UnmapBuffer();
	}

	// if this is a sub-allocation inside a larger buffer, don't actually free anything.
	if ( OwnsBuffer() == false ) {
		ClearWithoutFreeing();
		return;
	}

	if ( apiObject == 0 ) {
		return;
	}

	if ( r_showBuffers.GetBool() ) {
		idLib::Printf( "vertex buffer free %p, api %p (%i bytes)\n", this, GetAPIObject(), GetSize() );
	}

	GLuint bufferObject = apiObject;
	qglDeleteBuffersARB( 1, & bufferObject );

	ClearWithoutFreeing();
}

/*
========================
idVertexBuffer::Reference
========================
*/
void idVertexBuffer::Reference( const idVertexBuffer & other ) {
	assert( IsMapped() == false );
	//assert( other.IsMapped() == false );	// this happens when building idTriangles while at the same time setting up idDrawVerts
	assert( other.GetAPIObject() != NULL );
	assert( other.GetSize() > 0 );

	FreeBufferObject();
	size = other.GetSize();						// this strips the MAPPED_FLAG
	offsetInOtherBuffer = other.GetOffset();	// this strips the OWNS_BUFFER_FLAG
	apiObject = other.apiObject;
	assert( OwnsBuffer() == false );
}

/*
========================
idVertexBuffer::Reference
========================
*/
void idVertexBuffer::Reference( const idVertexBuffer & other, int refOffset, int refSize ) {
	assert( IsMapped() == false );
	//assert( other.IsMapped() == false );	// this happens when building idTriangles while at the same time setting up idDrawVerts
	assert( other.GetAPIObject() != NULL );
	assert( refOffset >= 0 );
	assert( refSize >= 0 );
	assert( refOffset + refSize <= other.GetSize() );

	FreeBufferObject();
	size = refSize;
	offsetInOtherBuffer = other.GetOffset() + refOffset;
	apiObject = other.apiObject;
	assert( OwnsBuffer() == false );
}

/*
========================
idVertexBuffer::Update
========================
*/
void idVertexBuffer::Update( const void * data, int updateSize ) const {
	assert( apiObject != NULL );
	assert( IsMapped() == false );
	assert_16_byte_aligned( data );
	assert( ( GetOffset() & 15 ) == 0 );

	if ( updateSize > size ) {
		idLib::FatalError( "idVertexBuffer::Update: size overrun, %i > %i\n", updateSize, GetSize() );
	}

	int numBytes = ( updateSize + 15 ) & ~15;

	GLuint bufferObject = apiObject;
	qglBindBufferARB( GL_ARRAY_BUFFER, bufferObject );
	qglBufferSubDataARB( GL_ARRAY_BUFFER, GetOffset(), (GLsizeiptrARB)numBytes, data );
/*
	void * buffer = MapBuffer( BM_WRITE );
	CopyBuffer( (byte *)buffer + GetOffset(), (byte *)data, numBytes );
	UnmapBuffer();
*/
}

/*
========================
idVertexBuffer::MapBuffer
========================
*/
void * idVertexBuffer::MapBuffer( bufferMapType_t mapType ) const {
	assert( apiObject != NULL );
	assert( IsMapped() == false );

	void * buffer = NULL;

	GLuint bufferObject = apiObject;
	qglBindBufferARB( GL_ARRAY_BUFFER, bufferObject );
	if ( mapType == BM_READ ) {
		buffer = qglMapBufferRange( GL_ARRAY_BUFFER, 0, GetAllocedSize(), GL_MAP_READ_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
		if ( buffer != NULL ) {
			buffer = (byte *)buffer + GetOffset();
		}
	} else if ( mapType == BM_WRITE ) {
		buffer = qglMapBufferRange( GL_ARRAY_BUFFER, 0, GetAllocedSize(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
		if ( buffer != NULL ) {
			buffer = (byte *)buffer + GetOffset();
		}
		assert( IsWriteCombined( buffer ) );
	} else {
		assert( false );
	}

	SetMapped();

	if ( buffer == NULL ) {
		idLib::FatalError( "idVertexBuffer::MapBuffer: failed" );
	}
	return buffer;
}

/*
========================
idVertexBuffer::UnmapBuffer
========================
*/
void idVertexBuffer::UnmapBuffer() const {
	assert( apiObject != NULL );
	assert( IsMapped() );

	GLuint bufferObject = apiObject;
	qglBindBufferARB( GL_ARRAY_BUFFER, bufferObject );
	if ( !qglUnmapBufferARB( GL_ARRAY_BUFFER ) ) {
		idLib::Printf( "idVertexBuffer::UnmapBuffer failed\n" );
	}

	SetUnmapped();
}

/*
================================================================================================

	idIndexBuffer

================================================================================================
*/

/*
========================
idIndexBuffer::~idIndexBuffer
========================
*/
idIndexBuffer::~idIndexBuffer() {
	FreeBufferObject();
}

/*
========================
idIndexBuffer::AllocBufferObject
========================
*/
bool idIndexBuffer::AllocBufferObject( const void * data, int allocSize ) {
	assert( apiObject == NULL );
	assert_16_byte_aligned( data );

	if ( allocSize <= 0 ) {
		idLib::Error( "idIndexBuffer::AllocBufferObject: allocSize = %i", allocSize );
	}

	size = allocSize;

	bool allocationFailed = false;

	int numBytes = GetAllocedSize();


	// clear out any previous error
	qglGetError();

	GLuint bufferObject = 0xFFFF;
	qglGenBuffersARB( 1, & bufferObject );
	if ( bufferObject == 0xFFFF ) {
		GLenum error = qglGetError();
		idLib::FatalError( "idIndexBuffer::AllocBufferObject: failed - GL_Error %d", error );
	}
	qglBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, bufferObject );

	// these are rewritten every frame
	qglBufferDataARB( GL_ELEMENT_ARRAY_BUFFER, numBytes, NULL, bufferUsage );
	apiObject = bufferObject;

	GLenum err = qglGetError();
	if ( err == GL_OUT_OF_MEMORY ) {
		idLib::Warning( "idIndexBuffer:AllocBufferObject: allocation failed" );
		allocationFailed = true;
	}


	if ( r_showBuffers.GetBool() ) {
		idLib::Printf( "index buffer alloc %p, api %p (%i bytes)\n", this, GetAPIObject(), GetSize() );
	}

	// copy the data
	if ( data != NULL ) {
		Update( data, allocSize );
	}

	return !allocationFailed;
}

/*
========================
idIndexBuffer::FreeBufferObject
========================
*/
void idIndexBuffer::FreeBufferObject() {
	if ( IsMapped() ) {
		UnmapBuffer();
	}

	// if this is a sub-allocation inside a larger buffer, don't actually free anything.
	if ( OwnsBuffer() == false ) {
		ClearWithoutFreeing();
		return;
	}

	if ( apiObject == 0 ) {
		return;
	}

	if ( r_showBuffers.GetBool() ) {
		idLib::Printf( "index buffer free %p, api %p (%i bytes)\n", this, GetAPIObject(), GetSize() );
	}

	GLuint bufferObject = apiObject;
	qglDeleteBuffersARB( 1, & bufferObject );

	ClearWithoutFreeing();
}

/*
========================
idIndexBuffer::Reference
========================
*/
void idIndexBuffer::Reference( const idIndexBuffer & other ) {
	assert( IsMapped() == false );
	//assert( other.IsMapped() == false );	// this happens when building idTriangles while at the same time setting up triIndex_t
	assert( other.GetAPIObject() != NULL );
	assert( other.GetSize() > 0 );

	FreeBufferObject();
	size = other.GetSize();						// this strips the MAPPED_FLAG
	offsetInOtherBuffer = other.GetOffset();	// this strips the OWNS_BUFFER_FLAG
	apiObject = other.apiObject;
	assert( OwnsBuffer() == false );
}

/*
========================
idIndexBuffer::Reference
========================
*/
void idIndexBuffer::Reference( const idIndexBuffer & other, int refOffset, int refSize ) {
	assert( IsMapped() == false );
	//assert( other.IsMapped() == false );	// this happens when building idTriangles while at the same time setting up triIndex_t
	assert( other.GetAPIObject() != NULL );
	assert( refOffset >= 0 );
	assert( refSize >= 0 );
	assert( refOffset + refSize <= other.GetSize() );

	FreeBufferObject();
	size = refSize;
	offsetInOtherBuffer = other.GetOffset() + refOffset;
	apiObject = other.apiObject;
	assert( OwnsBuffer() == false );
}

/*
========================
idIndexBuffer::Update
========================
*/
void idIndexBuffer::Update( const void * data, int updateSize ) const {

	assert( apiObject != NULL );
	assert( IsMapped() == false );
	assert_16_byte_aligned( data );
	assert( ( GetOffset() & 15 ) == 0 );

	if ( updateSize > size ) {
		idLib::FatalError( "idIndexBuffer::Update: size overrun, %i > %i\n", updateSize, GetSize() );
	}

	int numBytes = ( updateSize + 15 ) & ~15;

	GLuint bufferObject = apiObject;
	qglBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, bufferObject );
	qglBufferSubDataARB( GL_ELEMENT_ARRAY_BUFFER, GetOffset(), (GLsizeiptrARB)numBytes, data );
/*
	void * buffer = MapBuffer( BM_WRITE );
	CopyBuffer( (byte *)buffer + GetOffset(), (byte *)data, numBytes );
	UnmapBuffer();
*/
}

/*
========================
idIndexBuffer::MapBuffer
========================
*/
void * idIndexBuffer::MapBuffer( bufferMapType_t mapType ) const {

	assert( apiObject != NULL );
	assert( IsMapped() == false );

	void * buffer = NULL;

	GLuint bufferObject = apiObject;
	qglBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, bufferObject );
	if ( mapType == BM_READ ) {
		buffer = qglMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, GetAllocedSize(), GL_MAP_READ_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
		if ( buffer != NULL ) {
			buffer = (byte *)buffer + GetOffset();
		}
	} else if ( mapType == BM_WRITE ) {
		buffer = qglMapBufferRange( GL_ELEMENT_ARRAY_BUFFER, 0, GetAllocedSize(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
		if ( buffer != NULL ) {
			buffer = (byte *)buffer + GetOffset();
		}
		assert( IsWriteCombined( buffer ) );
	} else {
		assert( false );
	}

	SetMapped();

	if ( buffer == NULL ) {
		idLib::FatalError( "idIndexBuffer::MapBuffer: failed" );
	}
	return buffer;
}

/*
========================
idIndexBuffer::UnmapBuffer
========================
*/
void idIndexBuffer::UnmapBuffer() const {
	assert( apiObject != NULL );
	assert( IsMapped() );

	GLuint bufferObject = apiObject;
	qglBindBufferARB( GL_ELEMENT_ARRAY_BUFFER, bufferObject );
	if ( !qglUnmapBufferARB( GL_ELEMENT_ARRAY_BUFFER ) ) {
		idLib::Printf( "idIndexBuffer::UnmapBuffer failed\n" );
	}

	SetUnmapped();
}

/*
================================================================================================

	idUniformBuffer

================================================================================================
*/

/*
========================
idUniformBuffer::idUniformBufferer
========================
*/
idUniformBuffer::~idUniformBuffer() {
	FreeBufferObject();
}

/*
========================
idUniformBuffer::AllocBufferObject
========================
*/
bool idUniformBuffer::AllocBufferObject( const void * data, int allocSize ) {
	assert( apiObject == NULL );
	assert_16_byte_aligned( data );

	if ( allocSize <= 0 ) {
		idLib::Error( "idUniformBuffer::AllocBufferObject: allocSize = %i", allocSize );
	}

	size = allocSize;

	bool allocationFailed = false;

	const int numBytes = GetAllocedSize();

	GLuint buffer = 0;
	qglGenBuffersARB( 1, &buffer );
	qglBindBufferARB( GL_UNIFORM_BUFFER, buffer );
	qglBufferDataARB( GL_UNIFORM_BUFFER, numBytes, NULL, GL_STREAM_DRAW );
	qglBindBufferARB( GL_UNIFORM_BUFFER, 0);
	apiObject = buffer;

	if ( r_showBuffers.GetBool() ) {
		idLib::Printf( "uniform buffer alloc %p, api %p (%i bytes)\n", this, GetAPIObject(), GetSize() );
	}

	// copy the data
	if ( data != NULL ) {
		Update( data, allocSize );
	}

	return !allocationFailed;
}

/*
========================
idUniformBuffer::FreeBufferObject
========================
*/
void idUniformBuffer::FreeBufferObject() {
	if ( IsMapped() ) {
		UnmapBuffer();
	}

	// if this is a sub-allocation inside a larger buffer, don't actually free anything.
	if ( OwnsBuffer() == false ) {
		ClearWithoutFreeing();
		return;
	}

	if ( apiObject == 0 ) {
		return;
	}

	if ( r_showBuffers.GetBool() ) {
		idLib::Printf( "uniform buffer free %p, api %p (%i bytes)\n", this, GetAPIObject(), GetSize() );
	}

	GLuint buffer = apiObject;
	qglBindBufferARB( GL_UNIFORM_BUFFER, 0 );
	qglDeleteBuffersARB( 1, & buffer );

	ClearWithoutFreeing();
}

/*
========================
idUniformBuffer::Reference
========================
*/
void idUniformBuffer::Reference( const idUniformBuffer & other ) {
	assert( IsMapped() == false );
	assert( other.IsMapped() == false );
	assert( other.GetAPIObject() != NULL );
	assert(other.GetSize() > 0 );

	FreeBufferObject();
	size = other.GetSize();			// this strips the MAPPED_FLAG
	offsetInOtherBuffer = other.GetOffset();	// this strips the OWNS_BUFFER_FLAG
	apiObject = other.apiObject;
	assert( OwnsBuffer() == false );
}

/*
========================
idUniformBuffer::Reference
========================
*/
void idUniformBuffer::Reference( const idUniformBuffer & other, int refOffset, int refSize ) {
	assert( IsMapped() == false );
	assert( other.IsMapped() == false );
	assert( other.GetAPIObject() != NULL );
	assert( refOffset >= 0 );
	assert( refSize >= 0 );
	assert( refOffset + refSize <= other.GetSize() );

	FreeBufferObject();
	size = refSize;
	offsetInOtherBuffer = other.GetOffset() + refOffset;
	apiObject = other.apiObject;
	assert( OwnsBuffer() == false );
}

/*
========================
idUniformBuffer::Update
========================
*/
void idUniformBuffer::Update( const void * data, int updateSize ) const {
	assert( apiObject != NULL );
	assert( IsMapped() == false );
	assert_16_byte_aligned( data );
	assert( ( GetOffset() & 15 ) == 0 );

	if ( updateSize > size ) {
		idLib::FatalError( "idUniformBuffer::Update: size overrun, %i > %i\n", updateSize, GetSize() );
	}

	int numBytes = ( updateSize + 15 ) & ~15;

	GLuint bufferObject = apiObject;
	qglBindBufferARB( GL_UNIFORM_BUFFER, bufferObject );
	qglBufferSubDataARB( GL_UNIFORM_BUFFER, GetOffset(), (GLsizeiptrARB)numBytes, data );
}

/*
========================
idUniformBuffer::MapBuffer
========================
*/
void * idUniformBuffer::MapBuffer( bufferMapType_t mapType ) const {
	assert( IsMapped() == false );
	assert( mapType == BM_WRITE );
	assert( apiObject != NULL );

	int numBytes = GetAllocedSize();

	void * buffer = NULL;

	qglBindBufferARB( GL_UNIFORM_BUFFER, apiObject );
	numBytes = numBytes;
	assert( GetOffset() == 0 );
	//buffer = qglMapBufferARB( GL_UNIFORM_BUFFER, GL_WRITE_ONLY_ARB );
	buffer = qglMapBufferRange( GL_UNIFORM_BUFFER, 0, GetAllocedSize(), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT );
	if ( buffer != NULL ) {
		buffer = (byte *)buffer + GetOffset();
	}

	SetMapped();

	if ( buffer == NULL ) {
		idLib::FatalError( "idUniformBuffer::MapBuffer: failed" );
	}
	return (float *) buffer;
}

/*
========================
idUniformBuffer::UnmapBuffer
========================
*/
void idUniformBuffer::UnmapBuffer() const {
	assert( apiObject != NULL );
	assert( IsMapped() );

	qglBindBufferARB( GL_UNIFORM_BUFFER, apiObject );
	if ( !qglUnmapBufferARB( GL_UNIFORM_BUFFER ) ) {
		idLib::Printf( "idUniformBuffer::UnmapBuffer failed\n" );
	}

	SetUnmapped();
}
