/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/gl/gl.h"
#include "cinder/TriMesh.h"

#include <vector>
#include <utility>

namespace cinder { namespace gl {

//! Represents an OpenGL vbo object \ImplShared
class Vbo {
 public:
	Vbo() {}
	Vbo( GLenum aTarget );
	
	void		bind();
	void		unbind();
	
	void		bufferData( size_t size, const void *data, GLenum usage );
	void		bufferSubData( ptrdiff_t offset, size_t size, const void *data );
	
	uint8_t*	map( GLenum access );
	void		unmap();

	GLenum		getTarget() const { return mObj->mTarget; }
	GLuint		getId() const { return mObj->mId; }
	
 protected:
	struct Obj {
		Obj( GLenum aTarget );
		~Obj();

		GLenum			mTarget;
		GLuint			mId;
	};
	
	std::shared_ptr<Obj>	mObj;

  public:
	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> Vbo::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &Vbo::mObj; }
	void reset() { mObj.reset(); }
	//@}  
};

//! Represents a complete mesh stored as a set of OpenGL vbos \ImplShared
class VboMesh {
 public:
	enum { NONE, STATIC, DYNAMIC };
	enum { ATTR_INDICES, ATTR_POSITIONS, ATTR_NORMALS, ATTR_COLORS_RGB, ATTR_COLORS_RGBA, ATTR_TEXCOORDS2D_0, ATTR_TEXCOORDS2D_1, ATTR_TEXCOORDS2D_2, ATTR_TEXCOORDS2D_3, ATTR_TEXCOORDS3D_0, ATTR_TEXCOORDS3D_1, ATTR_TEXCOORDS3D_2, ATTR_TEXCOORDS3D_3, ATTR_TOTAL };
	enum { ATTR_MAX_TEXTURE_UNIT = 3 };

	//! Represents a configuration state for the VboMesh object
	struct Layout {
		Layout() { initAttributes(); }

		//! \return is the Layout unspecified, presumably TBG by a constructor for VboMesh
		bool	isDefaults() const { for( int a = 0; a < ATTR_TOTAL; ++a ) if( mAttributes[a] != NONE ) return false; return true; }
	
		//! \return true if the format object defines normals
		bool	hasNormals() const { return hasDynamicNormals() || hasStaticNormals(); }
		//! \return true if the format object defines static normal data
		bool	hasStaticNormals() const { return mAttributes[ATTR_NORMALS] == STATIC; }
		//! \return true if the format object defines dynamic normal data
		bool	hasDynamicNormals() const { return mAttributes[ATTR_NORMALS] == DYNAMIC; }
		//! assigns to the normals the static attribute
		void	setStaticNormals() { mAttributes[ATTR_NORMALS] = STATIC; }
		//! assigns to the normals the dynamoic attribute
		void	setDynamicNormals() { mAttributes[ATTR_NORMALS] = DYNAMIC; }		

		//! \return true if the format object defines colors using three channels (RGB)
		bool	hasColorsRGB() const { return hasDynamicColorsRGB() || hasStaticColorsRGB(); }
		//! \return true if the format object defines static RGB color data
		bool	hasStaticColorsRGB() const { return mAttributes[ATTR_COLORS_RGB] == STATIC; }
		//! \return true if the format object defines dynamic RGB color data
		bool	hasDynamicColorsRGB() const { return mAttributes[ATTR_COLORS_RGB] == DYNAMIC; }
		//! assigns to the RGB colors the static attribute
		void	setStaticColorsRGB() { mAttributes[ATTR_COLORS_RGB] = STATIC; mAttributes[ATTR_COLORS_RGBA] = NONE; }
		//! assigns to the RGB colors the dynamic attribute
		void	setDynamicColorsRGB() { mAttributes[ATTR_COLORS_RGB] = DYNAMIC; mAttributes[ATTR_COLORS_RGBA] = NONE; }		

		//! \return true if the format object defines colors using four channels (RGBA)
		bool	hasColorsRGBA() const { return hasDynamicColorsRGBA() || hasStaticColorsRGBA(); }
		//! \return true if the format object defines static RGBA color data
		bool	hasStaticColorsRGBA() const { return mAttributes[ATTR_COLORS_RGBA] == STATIC; }
		//! \return true if the format object defines dynamic RGBA color data
		bool	hasDynamicColorsRGBA() const { return mAttributes[ATTR_COLORS_RGBA] == DYNAMIC; }
		//! assigns to the RGBA colors the static attribute
		void	setStaticColorsRGBA() { mAttributes[ATTR_COLORS_RGBA] = STATIC; mAttributes[ATTR_COLORS_RGB] = NONE; }
		//! assigns to the RGBA colors the static attribute
		void	setDynamicColorsRGBA() { mAttributes[ATTR_COLORS_RGBA] = DYNAMIC; mAttributes[ATTR_COLORS_RGB] = NONE; }		
		
		//! \return true if 2d texture coordinates are defined for the given texture unit (defaults to zero)
		bool	hasTexCoords2d( size_t unit = 0 ) const { return hasDynamicTexCoords2d( unit ) || hasStaticTexCoords2d( unit ); }
		//! \return true if 2d texture coordinates are defined as static data for the given texture unit (defaults to zero)
		bool	hasStaticTexCoords2d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS2D_0 + unit] == STATIC; }		
		//! \return true if 2d texture coordinates are defined as dynamic data for the given texture unit (defaults to zero)
		bool	hasDynamicTexCoords2d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS2D_0 + unit] == DYNAMIC; }
		//! sets the 2d texture coordinates as static data for the given texture unit (defaults to zero)
		void	setStaticTexCoords2d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS2D_0 + unit] = STATIC; mAttributes[ATTR_TEXCOORDS3D_0 + unit] = NONE; }
		//! sets the 2d texture coordinates as dynamic data for the given texture unit (defaults to zero)
		void	setDynamicTexCoords2d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS2D_0 + unit] = DYNAMIC; mAttributes[ATTR_TEXCOORDS3D_0 + unit] = NONE; }
		//! \return are there any texture units with static texCoords
		bool	hasStaticTexCoords() const;
		//! \return are there any texture units with dynamic texCoords
		bool	hasDynamicTexCoords() const;
		//! \return if texture unit \arg unit is enabled
		bool	hasTexCoords( size_t unit ) const { return ( mAttributes[ATTR_TEXCOORDS2D_0 + unit] != NONE ) || ( mAttributes[ATTR_TEXCOORDS3D_0 + unit] != NONE ); }

		//! \return true if 3d texture coordinates are defined for the given texture unit (defaults to zero)
		bool	hasTexCoords3d( size_t unit = 0 ) const { return hasDynamicTexCoords3d( unit ) || hasStaticTexCoords3d( unit ); }
		//! \return true if 2d texture coordinates are defined as static data for the given texture unit (defaults to zero)
		bool	hasStaticTexCoords3d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS3D_0 + unit] == STATIC; }		
		//! \return true if 2d texture coordinates are defined as dynamic data for the given texture unit (defaults to zero)
		bool	hasDynamicTexCoords3d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS3D_0 + unit] == DYNAMIC; }
		//! sets the 2d texture coordinates as static data for the given texture unit (defaults to zero)
		void	setStaticTexCoords3d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS3D_0 + unit] = STATIC; mAttributes[ATTR_TEXCOORDS2D_0 + unit] = NONE; }
		//! sets the 2d texture coordinates as dynamic data for the given texture unit (defaults to zero)
		void	setDynamicTexCoords3d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS3D_0 + unit] = DYNAMIC; mAttributes[ATTR_TEXCOORDS2D_0 + unit] = NONE; }

		//! \return true if indices are defined for the vertex data
		bool	hasIndices() const { return hasStaticIndices() || hasDynamicIndices(); }
		//! \return true if indices are defined for the vertex data
		bool	hasStaticIndices() const { return mAttributes[ATTR_INDICES] == STATIC; }
		//! \return true if the vertex indices are defined as static data
		bool	hasDynamicIndices() const { return mAttributes[ATTR_INDICES] == DYNAMIC; }
		//! assigns to the vertex index the static attribute
		void	setStaticIndices() { mAttributes[ATTR_INDICES] = STATIC; }
		//! assigns to the vertex index the dynamic attribute
		void	setDynamicIndices() { mAttributes[ATTR_INDICES] = DYNAMIC; }

		//! \return true if the vertex positions are defined
		bool	hasPositions() const { return hasStaticPositions() || hasDynamicPositions(); }
		//! \return true if the vertex positions are defined as static data
		bool	hasStaticPositions() const { return mAttributes[ATTR_POSITIONS] == STATIC; }
		//! \return true if the vertex positions are defined as dynamic data
		bool	hasDynamicPositions() const { return mAttributes[ATTR_POSITIONS] == DYNAMIC; }
		//! assigns to the vertex positions the static attribute
		void	setStaticPositions() { mAttributes[ATTR_POSITIONS] = STATIC; }
		//! assigns to the vertex positions the dynamic attribute
		void	setDynamicPositions() { mAttributes[ATTR_POSITIONS] = DYNAMIC; }
		
		enum CustomAttr { CUSTOM_ATTR_FLOAT, CUSTOM_ATTR_FLOAT2, CUSTOM_ATTR_FLOAT3, CUSTOM_ATTR_FLOAT4, TOTAL_CUSTOM_ATTR_TYPES };
		static int sCustomAttrSizes[TOTAL_CUSTOM_ATTR_TYPES];
		static GLint sCustomAttrNumComponents[TOTAL_CUSTOM_ATTR_TYPES];
		static GLenum sCustomAttrTypes[TOTAL_CUSTOM_ATTR_TYPES];
		
		//! adds floating point data to the custom defined vertex attributes
		void	addDynamicCustomFloat() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT, 0 ) ); }
		//! adds 2d float vector data to the custom defined vertex attributes
		void	addDynamicCustomVec2f() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT2, 0 ) ); }
		//! adds 3d float vector data to the custom defined vertex attributes
		void	addDynamicCustomVec3f() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT3, 0 ) ); }
		//! adds 4d float vector data to the custom defined vertex attributes
		void	addDynamicCustomVec4f() { mCustomDynamic.push_back( std::make_pair( CUSTOM_ATTR_FLOAT4, 0 ) ); }

		int												mAttributes[ATTR_TOTAL];
		std::vector<std::pair<CustomAttr,size_t> >		mCustomDynamic, mCustomStatic; // pair of <types,offset>
		
	 private:
		void initAttributes() { for( int a = 0; a < ATTR_TOTAL; ++a ) mAttributes[a] = NONE; }
	};

	enum			{ INDEX_BUFFER = 0, STATIC_BUFFER, DYNAMIC_BUFFER, TOTAL_BUFFERS };
	
  protected:
	struct Obj {
		size_t			mNumIndices, mNumVertices;	

		Vbo				mBuffers[TOTAL_BUFFERS];
		size_t			mPositionOffset;
		size_t			mNormalOffset;
		size_t			mColorRGBOffset, mColorRGBAOffset;		
		size_t			mTexCoordOffset[ATTR_MAX_TEXTURE_UNIT+1];
		size_t			mStaticStride, mDynamicStride;	
		GLenum			mPrimitiveType;
		Layout			mLayout;
		std::vector<GLint>		mCustomStaticLocations;
		std::vector<GLint>		mCustomDynamicLocations;
	};

  public:
	class VertexIter;
 
	VboMesh() {}
	/*** Creates a VboMesh instance using the input TriMesh object to initialize the OpenGL buffers **/
	explicit VboMesh( const TriMesh &triMesh, Layout layout = Layout() );
	/*** Creates a VboMesh instance using the input TriMesh2d object to initialize the OpenGL buffers **/
	explicit VboMesh( const TriMesh2d &triMesh, Layout layout = Layout() );
	/*** Creates a VboMesh with \a numVertices vertices and \a numIndices indices. Dynamic data is stored interleaved and static data is planar. **/
	VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType );
	/*** Creates a VboMesh with \a numVertices vertices and \a numIndices indices. Accepts pointers to preexisting buffers, which may be NULL to request allocation **/
	VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType, Vbo *indexBuffer, Vbo *staticBuffer, Vbo *dynamicBuffer );

	//! \return the number of vertex indices defined in the index buffer
	size_t	getNumIndices() const { return mObj->mNumIndices; }
	//! \return the number of vertex positions defined in the vertex buffer
	size_t	getNumVertices() const { return mObj->mNumVertices; }
	//! \return the primitive type defined for the vertex buffer object (GL_TRIANGLES, GL_QUADS, GL_LINES, etc)
	GLenum	getPrimitiveType() const { return mObj->mPrimitiveType; }
	
	//! \return a constant reference to the internally stored layout object
	const Layout&	getLayout() const { return mObj->mLayout; }

	//! Binds the vertex index buffer
	void			bindIndexBuffer() const;
	//! Enables all client states associated with the VboMesh object
	void			enableClientStates() const;
	//! Disables all client states associated with the VboMesh object
	void			disableClientStates() const;
	//! Binds all buffer objects for drawing
	void			bindAllData() const;
	//! Unbinds all vertex buffer objects
	static void		unbindBuffers();

	//! Buffers index data from an input STL vector container
	void						bufferIndices( const std::vector<uint32_t> &indices );
	//! Buffers vertex position data from an input STL vector container
	void						bufferPositions( const std::vector<Vec3f> &positions );
	//! Buffers vertex position data from an array of Vec3f objects
	void						bufferPositions( const Vec3f *positions, size_t count );
	//! Buffers normal vector data from an input STL vector container
	void						bufferNormals( const std::vector<Vec3f> &normals );
	//! Buffers 2d texture coordinate data to the input texture unit from an input STL vector container
	void						bufferTexCoords2d( size_t unit, const std::vector<Vec2f> &texCoords );
	//! Buffers 3d texture coordinate data to the input texture unit from an input STL vector container
	void						bufferTexCoords3d( size_t unit, const std::vector<Vec3f> &texCoords );
	//! Buffers RGB color data from an input STL vector container
	void						bufferColorsRGB( const std::vector<Color> &colors );
	//! Buffers RGBA color data from an input STL vector container
	void						bufferColorsRGBA( const std::vector<ColorA> &colors );
	//! \return a vertex iterator to the mapped buffer object
	class VertexIter			mapVertexBuffer();

	//! \return a reference to the vbo object that stores the vertex index data
	Vbo&				getIndexVbo() const { return mObj->mBuffers[INDEX_BUFFER]; }
	//! \return a reference to the vbo object that stores the static data
	Vbo&				getStaticVbo() const { return mObj->mBuffers[STATIC_BUFFER]; }
	//! \return a reference to the vbo object that stores the dynamic data
	Vbo&				getDynamicVbo() const { return mObj->mBuffers[DYNAMIC_BUFFER]; }

	//! assigns a custom location for statically buffered data
	void				setCustomStaticLocation( size_t internalIndex, GLuint location ) { mObj->mCustomStaticLocations[internalIndex] = location; }
	//! assigns a custom location for dynamically buffered data
	void				setCustomDynamicLocation( size_t internalIndex, GLuint location ) { mObj->mCustomDynamicLocations[internalIndex] = location; }

	//! \return the texture coordinate offset for the given texture unit
	size_t						getTexCoordOffset( size_t unit ) const { return mObj->mTexCoordOffset[unit]; }
	//! assigns the texture coordinate offset for the given texture unit
	void						setTexCoordOffset( size_t unit, size_t aTexCoordOffset ) { mObj->mTexCoordOffset[unit] = aTexCoordOffset; }	

	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> VboMesh::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &VboMesh::mObj; }
	void reset() { mObj.reset(); }
	//@}

	//! Defines an iterator type for Vertex data stored in a VboMesh
	class VertexIter {
	 public:
		VertexIter( const VboMesh &mesh );

		//! assigns a new position to the current vertex
		void	setPosition( const Vec3f &v ) { *(reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] )) = v; }
		//! assigns a new position to the current vertex
		void	setPosition( float x, float y, float z ) { *(reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] )) = Vec3f( x, y, z ); }
		//! assigns a new normal to the current vertex
		void	setNormal( const Vec3f &n ) { *(reinterpret_cast<Vec3f*>( &mPtr[mNormalOffset] )) = n; }
		//! assigns a new RGB color to the current vertex
		void	setColorRGB( const Color &n ) { *(reinterpret_cast<Color*>( &mPtr[mColorRGBOffset] )) = n; }
		//! assigns a new RGBA color to the current vertex
		void	setColorRGBA( const ColorA &n ) { *(reinterpret_cast<ColorA*>( &mPtr[mColorRGBAOffset] )) = n; }
		//! assigns a new 2d texture coordinate on the first texture unit to the current vertex
		void	setTexCoord2d0( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[0]] )) = t; }
		//! assigns a new 3d texture coordinate on the first texture unit to the current vertex
		void	setTexCoord3d0( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[0]] )) = t; }
		//! assigns a new 2d texture coordinate on the second texture unit to the current vertex
		void	setTexCoord2d1( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[1]] )) = t; }
		//! assigns a new 3d texture coordinate on the second texture unit to the current vertex
		void	setTexCoord3d1( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[1]] )) = t; }
		//! assigns a new 2d texture coordinate on the third texture unit to the current vertex
		void	setTexCoord2d2( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[2]] )) = t; }
		//! assigns a new 3d texture coordinate on the third texture unit to the current vertex
		void	setTexCoord3d2( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[2]] )) = t; }
		//! assigns a new floating point value as a custom attribute to the current vertex
		void	setCustomFloat( size_t index, float v ) { *(reinterpret_cast<float*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		//! assigns a new 2d vector as a custom attribute to the current vertex
		void	setCustomVec2f( size_t index, const Vec2f &v ) { *(reinterpret_cast<Vec2f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		//! assigns a new 3d vector as a custom attribute to the current vertex
		void	setCustomVec3f( size_t index, const Vec3f &v ) { *(reinterpret_cast<Vec3f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		//! assigns a new 4d vector as a custom attribute to the current vertex
		void	setCustomVec4f( size_t index, const Vec4f &v ) { *(reinterpret_cast<Vec4f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }

		//! advances pointer to the next vertex
		void operator++() { mPtr += mStride; }
		//! \return true if the iterator may be advanced
		bool	isDone() const { return mPtr < mDataEnd; }
		
		//! \return Which vertex the iterator is pointing to
		size_t		getIndex() const { return ( mPtr - mData ) / mStride; }
		//! \return Which vertex the iterator is pointing to
		size_t		getStride() const { return mStride; }
		//! \return Raw pointer where the iterator is currently writing
		void*		getPointer() const { return reinterpret_cast<void*>( mPtr ); }
		//! \return pointer where the iterator is currently writing positions
		Vec3f*		getPositionPointer() const { return reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] ); }		

//		VertexIter( const VertexIter &other ) { set( other ); }	
//		VertexIter& operator=( const VertexIter &other ) { set( other ); return *this; }
		
	 protected:
		void set( const VertexIter &other );

		struct Obj {
		 public:
			Obj( const VboMesh &mesh );
			~Obj();
			
			uint8_t					*mData, *mDataEnd;
			std::vector<size_t>		mCustomOffsets;			
			Vbo						mVbo;
		};
	 	 
		std::shared_ptr<Obj>	mObj;
		uint8_t					*mPtr;
		uint8_t					*mData, *mDataEnd; // we cache these from the Obj to reduce dereferencing
		size_t					mPositionOffset, mNormalOffset;
		size_t					mColorRGBOffset, mColorRGBAOffset;
		size_t					mTexCoordOffset[ATTR_MAX_TEXTURE_UNIT+1];
		uint8_t					mStride;
	};

 protected:
	void	initializeBuffers( bool staticDataPlanar );

	std::shared_ptr<Obj>		mObj;
};

class VboExc : public std::exception {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception"; }
};

class VboInvalidTargetExc : public VboExc {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception: Invalid Target"; }
};

class VboFailedMapExc : public VboExc {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception: Map failure"; } 
};

class VboFailedUnmapExc : public VboExc {
 public:
	virtual const char* what() const throw() { return "OpenGL Vbo exception: Unmap failure"; } 
};

} } // namespace cinder::gl
