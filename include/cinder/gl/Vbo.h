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

/** Represents an OpenGL Vertex Buffer Object \ImplShared */
class Vbo {
 public:
	/** Creates an uninitialized empty vertex buffer object */
	Vbo() {}
	/** Creates a GL vertex buffer object by invoking <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glGenBuffers.xml">glGenBuffers</a> */
	Vbo( GLenum aTarget );

	//! Binds the OpenGL vertex buffer object using <a href="http://www.opengl.org/sdk/docs/man3/xhtml/glBindBuffer.xml">glBindBuffer</a>
	void		bind();
	//! Unbinds the OpenGL vertex buffer object 
	void		unbind();
	
	//! Loads data into the buffer using <a href="http://www.opengl.org/sdk/docs/man/xhtml/glBufferData.xml">glBufferDataARB</a>
	void		bufferData( size_t size, const void *data, GLenum usage );
	//! Loads data into the buffer using <a href="http://www.opengl.org/sdk/docs/man/xhtml/glBufferSubData.xml">glBufferSubDataARB</a>
	void		bufferSubData( ptrdiff_t offset, size_t size, const void *data );
	
	//! Returns a pointer to the raw VBO buffer data using <a href="http://www.opengl.org/sdk/docs/man/xhtml/glMapBuffer.xml">glMapBuffer</a>
	uint8_t*	map( GLenum access );
	//! Un-maps the mapped VBO buffer using glUnmapBuffer
	void		unmap();

	//! Returns the GL target for the VBO (GL_ARRAY_BUFFER, GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, GL_ELEMENT_ARRAY_BUFFER, etc...)
	GLenum		getTarget() const { return mObj->mTarget; }
	//! Returns the id representing the GL VBO
	GLuint		getId() const { return mObj->mId; }
	
 protected:
	/** Internal shared implementation for the vertex buffer object */
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

//! Represents a complete mesh stored as a set of OpenGL Vertex Buffer Objects
/** 
 * The VboMesh object contains a static and dynamic Vbos which are used to draw mesh data. 
 * The mesh data is described using a VboMesh::Layout object. If the object is constructed 
 * from a TriMesh or TriMesh2d then the layout can be inferred. However, if the raw vertex
 * data is being loaded manually then the Layout object is used to describe what data is
 * defined. <br/>
 * 
 * The VboMesh also defines the VertexIter type which exposes the internal mesh data for 
 * modification using mapped dynamic buffers.<br/>
 * 
 * Example using an externally loaded OBJ file:<br/>
\code
using namespace ci;

ObjLoader loader( (DataSourceRef) app::loadResource( RES_CUBE_OBJ ) );
TriMesh mesh;
loader.load( &mesh );
gl::VboMesh vbo_mesh( mesh );
gl::draw(vbo_mesh);
\endcode
 * <br/>
 * Example using raw vertex data:<br/>
\code
using namespace ci;
using namespace std;

// setup the parameters of the Vbo
static const int VERTICES_X = 250, VERTICES_Z = 50;

int totalVertices = VERTICES_X * VERTICES_Z;
int totalQuads = ( VERTICES_X - 1 ) * ( VERTICES_Z - 1 );
gl::VboMesh::Layout layout;
layout.setStaticIndices();
layout.setDynamicPositions();
layout.setStaticTexCoords2d();
gl::VboMesh vboMesh( totalVertices, totalQuads * 4, layout, GL_QUADS );

// buffer our static data - the texcoords and the indices
vector<uint32_t> indices;
vector<Vec2f> texCoords;
for( int x = 0; x < VERTICES_X; ++x ) {
	for( int z = 0; z < VERTICES_Z; ++z ) {
		// create a quad for each vertex, except for along the bottom and right edges
		if( ( x + 1 < VERTICES_X ) && ( z + 1 < VERTICES_Z ) ) {
			indices.push_back( (x+0) * VERTICES_Z + (z+0) );
			indices.push_back( (x+1) * VERTICES_Z + (z+0) );
			indices.push_back( (x+1) * VERTICES_Z + (z+1) );
			indices.push_back( (x+0) * VERTICES_Z + (z+1) );
		}
		// the texture coordinates are mapped to [0,1.0)
		texCoords.push_back( Vec2f( x / (float)VERTICES_X, z / (float)VERTICES_Z ) );
	}
}

vboMesh.bufferIndices( indices );
vboMesh.bufferTexCoords2d( 0, texCoords );
\endcode
 * <br/>
 * The VboMesh and VertexIter objects are both designed with an \ImplShared <br/>
 * 
 */
class VboMesh {
 public:
	enum { NONE, STATIC, DYNAMIC };
	enum { ATTR_INDICES, ATTR_POSITIONS, ATTR_NORMALS, ATTR_COLORS_RGB, ATTR_COLORS_RGBA, ATTR_TEXCOORDS2D_0, ATTR_TEXCOORDS2D_1, ATTR_TEXCOORDS2D_2, ATTR_TEXCOORDS2D_3, ATTR_TEXCOORDS3D_0, ATTR_TEXCOORDS3D_1, ATTR_TEXCOORDS3D_2, ATTR_TEXCOORDS3D_3, ATTR_TOTAL };
	enum { ATTR_MAX_TEXTURE_UNIT = 3 };

	/** Represents a configuration state for the VboMesh object */
	struct Layout {
		Layout() { initAttributes(); }

		//! Returns is the Layout unspecified, presumably TBG by a constructor for VboMesh
		bool	isDefaults() const { for( int a = 0; a < ATTR_TOTAL; ++a ) if( mAttributes[a] != NONE ) return false; return true; }
	
		//! Returns true if the format object defines normals
		bool	hasNormals() const { return hasDynamicNormals() || hasStaticNormals(); }
		//! Returns true if the format object defines static normal data
		bool	hasStaticNormals() const { return mAttributes[ATTR_NORMALS] == STATIC; }
		//! Returns true if the format object defines dynamic normal data
		bool	hasDynamicNormals() const { return mAttributes[ATTR_NORMALS] == DYNAMIC; }
		//! Assigns to the normals the static attribute
		void	setStaticNormals() { mAttributes[ATTR_NORMALS] = STATIC; }
		//! Assigns to the normals the dynamoic attribute
		void	setDynamicNormals() { mAttributes[ATTR_NORMALS] = DYNAMIC; }		

		//! Returns true if the format object defines colors using three channels (RGB)
		bool	hasColorsRGB() const { return hasDynamicColorsRGB() || hasStaticColorsRGB(); }
		//! Returns true if the format object defines static RGB color data
		bool	hasStaticColorsRGB() const { return mAttributes[ATTR_COLORS_RGB] == STATIC; }
		//! Returns true if the format object defines dynamic RGB color data
		bool	hasDynamicColorsRGB() const { return mAttributes[ATTR_COLORS_RGB] == DYNAMIC; }
		//! Assigns to the RGB colors the static attribute
		void	setStaticColorsRGB() { mAttributes[ATTR_COLORS_RGB] = STATIC; mAttributes[ATTR_COLORS_RGBA] = NONE; }
		//! Assigns to the RGB colors the dynamic attribute
		void	setDynamicColorsRGB() { mAttributes[ATTR_COLORS_RGB] = DYNAMIC; mAttributes[ATTR_COLORS_RGBA] = NONE; }		

		//! Returns true if the format object defines colors using four channels (RGBA)
		bool	hasColorsRGBA() const { return hasDynamicColorsRGBA() || hasStaticColorsRGBA(); }
		//! Returns true if the format object defines static RGBA color data
		bool	hasStaticColorsRGBA() const { return mAttributes[ATTR_COLORS_RGBA] == STATIC; }
		//! Returns true if the format object defines dynamic RGBA color data
		bool	hasDynamicColorsRGBA() const { return mAttributes[ATTR_COLORS_RGBA] == DYNAMIC; }
		//! Assigns to the RGBA colors the static attribute
		void	setStaticColorsRGBA() { mAttributes[ATTR_COLORS_RGBA] = STATIC; mAttributes[ATTR_COLORS_RGB] = NONE; }
		//! Assigns to the RGBA colors the static attribute
		void	setDynamicColorsRGBA() { mAttributes[ATTR_COLORS_RGBA] = DYNAMIC; mAttributes[ATTR_COLORS_RGB] = NONE; }		
		
		//! Returns true if 2d texture coordinates are defined for the given texture unit (defaults to zero)
		bool	hasTexCoords2d( size_t unit = 0 ) const { return hasDynamicTexCoords2d( unit ) || hasStaticTexCoords2d( unit ); }
		//! Returns true if 2d texture coordinates are defined as static data for the given texture unit (defaults to zero)
		bool	hasStaticTexCoords2d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS2D_0 + unit] == STATIC; }		
		//! Returns true if 2d texture coordinates are defined as dynamic data for the given texture unit (defaults to zero)
		bool	hasDynamicTexCoords2d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS2D_0 + unit] == DYNAMIC; }
		//! sets the 2d texture coordinates as static data for the given texture unit (defaults to zero)
		void	setStaticTexCoords2d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS2D_0 + unit] = STATIC; mAttributes[ATTR_TEXCOORDS3D_0 + unit] = NONE; }
		//! sets the 2d texture coordinates as dynamic data for the given texture unit (defaults to zero)
		void	setDynamicTexCoords2d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS2D_0 + unit] = DYNAMIC; mAttributes[ATTR_TEXCOORDS3D_0 + unit] = NONE; }
		//! Returns are there any texture units with static texCoords
		bool	hasStaticTexCoords() const;
		//! Returns are there any texture units with dynamic texCoords
		bool	hasDynamicTexCoords() const;
		//! Returns if texture unit \arg unit is enabled
		bool	hasTexCoords( size_t unit ) const { return ( mAttributes[ATTR_TEXCOORDS2D_0 + unit] != NONE ) || ( mAttributes[ATTR_TEXCOORDS3D_0 + unit] != NONE ); }

		//! Returns true if 3d texture coordinates are defined for the given texture unit (defaults to zero)
		bool	hasTexCoords3d( size_t unit = 0 ) const { return hasDynamicTexCoords3d( unit ) || hasStaticTexCoords3d( unit ); }
		//! Returns true if 2d texture coordinates are defined as static data for the given texture unit (defaults to zero)
		bool	hasStaticTexCoords3d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS3D_0 + unit] == STATIC; }		
		//! Returns true if 2d texture coordinates are defined as dynamic data for the given texture unit (defaults to zero)
		bool	hasDynamicTexCoords3d( size_t unit = 0 ) const { return mAttributes[ATTR_TEXCOORDS3D_0 + unit] == DYNAMIC; }
		//! sets the 2d texture coordinates as static data for the given texture unit (defaults to zero)
		void	setStaticTexCoords3d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS3D_0 + unit] = STATIC; mAttributes[ATTR_TEXCOORDS2D_0 + unit] = NONE; }
		//! sets the 2d texture coordinates as dynamic data for the given texture unit (defaults to zero)
		void	setDynamicTexCoords3d( size_t unit = 0 ) { mAttributes[ATTR_TEXCOORDS3D_0 + unit] = DYNAMIC; mAttributes[ATTR_TEXCOORDS2D_0 + unit] = NONE; }

		//! Returns true if indices are defined for the vertex data
		bool	hasIndices() const { return hasStaticIndices() || hasDynamicIndices(); }
		//! Returns true if indices are defined for the vertex data
		bool	hasStaticIndices() const { return mAttributes[ATTR_INDICES] == STATIC; }
		//! Returns true if the vertex indices are defined as static data
		bool	hasDynamicIndices() const { return mAttributes[ATTR_INDICES] == DYNAMIC; }
		//! Assigns to the vertex index the static attribute
		void	setStaticIndices() { mAttributes[ATTR_INDICES] = STATIC; }
		//! Assigns to the vertex index the dynamic attribute
		void	setDynamicIndices() { mAttributes[ATTR_INDICES] = DYNAMIC; }

		//! Returns true if the vertex positions are defined
		bool	hasPositions() const { return hasStaticPositions() || hasDynamicPositions(); }
		//! Returns true if the vertex positions are defined as static data
		bool	hasStaticPositions() const { return mAttributes[ATTR_POSITIONS] == STATIC; }
		//! Returns true if the vertex positions are defined as dynamic data
		bool	hasDynamicPositions() const { return mAttributes[ATTR_POSITIONS] == DYNAMIC; }
		//! Assigns to the vertex positions the static attribute
		void	setStaticPositions() { mAttributes[ATTR_POSITIONS] = STATIC; }
		//! Assigns to the vertex positions the dynamic attribute
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
	/** Internal shared implementation for the VboMesh object */
	struct Obj {
		size_t				mNumIndices, mNumVertices;	

		Vbo					mBuffers[TOTAL_BUFFERS];
		size_t				mPositionOffset;
		size_t				mNormalOffset;
		size_t				mColorRGBOffset, mColorRGBAOffset;		
		size_t				mTexCoordOffset[ATTR_MAX_TEXTURE_UNIT+1];
		size_t				mStaticStride, mDynamicStride;	
		GLenum				mPrimitiveType;
		Layout				mLayout;
		std::vector<GLint>	mCustomStaticLocations;
		std::vector<GLint>	mCustomDynamicLocations;
	};

  public:
	class VertexIter;
	
	/** \brief Creates an empty VboMesh instance */
	VboMesh() {}
	/** \brief Creates a VboMesh instance using the input TriMesh object to initialize the OpenGL buffers */
	explicit VboMesh( const TriMesh &triMesh, Layout layout = Layout() );
	/** \brief Creates a VboMesh instance using the input TriMesh2d object to initialize the OpenGL buffers */
	explicit VboMesh( const TriMesh2d &triMesh, Layout layout = Layout() );
	/** \brief Creates a VboMesh with \a numVertices vertices and \a numIndices indices. Dynamic data is stored interleaved and static data is planar. */
	VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType );
	/** \brief Creates a VboMesh with \a numVertices vertices and \a numIndices indices. Accepts pointers to preexisting buffers, which may be NULL to request allocation */
	VboMesh( size_t numVertices, size_t numIndices, Layout layout, GLenum primitiveType, Vbo *indexBuffer, Vbo *staticBuffer, Vbo *dynamicBuffer );

	//! Returns the number of vertex indices defined in the index buffer
	size_t				getNumIndices() const { return mObj->mNumIndices; }
	//! Returns the number of vertex positions defined in the vertex buffer
	size_t				getNumVertices() const { return mObj->mNumVertices; }
	//! Returns the primitive type defined for the vertex buffer object (GL_TRIANGLES, GL_QUADS, GL_LINES, etc)
	GLenum				getPrimitiveType() const { return mObj->mPrimitiveType; }
	
	//! Returns a constant reference to the internally stored layout object
	const Layout&		getLayout() const { return mObj->mLayout; }

	//! Binds the vertex index buffer
	void				bindIndexBuffer() const;
	//! Enables all client states associated with the VboMesh object
	void				enableClientStates() const;
	//! Disables all client states associated with the VboMesh object
	void				disableClientStates() const;
	//! Binds all buffer objects for drawing
	void				bindAllData() const;
	//! Unbinds all vertex buffer objects
	static void			unbindBuffers();

	//! Buffers index data from an input STL vector container
	void				bufferIndices( const std::vector<uint32_t> &indices );
	//! Buffers vertex position data from an input STL vector container
	void				bufferPositions( const std::vector<Vec3f> &positions );
	//! Buffers vertex position data from an array of Vec3f objects
	void				bufferPositions( const Vec3f *positions, size_t count );
	//! Buffers normal vector data from an input STL vector container
	void				bufferNormals( const std::vector<Vec3f> &normals );
	//! Buffers 2d texture coordinate data to the input texture unit from an input STL vector container
	void				bufferTexCoords2d( size_t unit, const std::vector<Vec2f> &texCoords );
	//! Buffers 3d texture coordinate data to the input texture unit from an input STL vector container
	void				bufferTexCoords3d( size_t unit, const std::vector<Vec3f> &texCoords );
	//! Buffers RGB color data from an input STL vector container
	void				bufferColorsRGB( const std::vector<Color> &colors );
	//! Buffers RGBA color data from an input STL vector container
	void				bufferColorsRGBA( const std::vector<ColorA> &colors );
	//! Returns a vertex iterator to the mapped buffer object
	class VertexIter	mapVertexBuffer();

	//! Returns a reference to the vbo object that stores the vertex index data
	Vbo&				getIndexVbo() const { return mObj->mBuffers[INDEX_BUFFER]; }
	//! Returns a reference to the vbo object that stores the static data
	Vbo&				getStaticVbo() const { return mObj->mBuffers[STATIC_BUFFER]; }
	//! Returns a reference to the vbo object that stores the dynamic data
	Vbo&				getDynamicVbo() const { return mObj->mBuffers[DYNAMIC_BUFFER]; }

	//! Assigns a custom location for statically buffered data
	void				setCustomStaticLocation( size_t internalIndex, GLuint location ) { mObj->mCustomStaticLocations[internalIndex] = location; }
	//! Assigns a custom location for dynamically buffered data
	void				setCustomDynamicLocation( size_t internalIndex, GLuint location ) { mObj->mCustomDynamicLocations[internalIndex] = location; }

	//! Returns the texture coordinate offset for the given texture unit
	size_t				getTexCoordOffset( size_t unit ) const { return mObj->mTexCoordOffset[unit]; }
	//! Assigns the texture coordinate offset for the given texture unit
	void				setTexCoordOffset( size_t unit, size_t aTexCoordOffset ) { mObj->mTexCoordOffset[unit] = aTexCoordOffset; }	

	//@{
	//! Emulates shared_ptr-like behavior
	typedef std::shared_ptr<Obj> VboMesh::*unspecified_bool_type;
	operator unspecified_bool_type() const { return ( mObj.get() == 0 ) ? 0 : &VboMesh::mObj; }
	void reset() { mObj.reset(); }
	//@}

	/** Defines an iterator type for Vertex data stored in a VboMesh */
	class VertexIter {
	 public:
		/** Exposes write access to the buffered contents of a dynamic VBO */
		VertexIter( const VboMesh &mesh );

		//! Assigns a new position to the current vertex
		void	setPosition( const Vec3f &v ) { *(reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] )) = v; }
		//! Assigns a new position to the current vertex
		void	setPosition( float x, float y, float z ) { *(reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] )) = Vec3f( x, y, z ); }
		//! Assigns a new normal to the current vertex
		void	setNormal( const Vec3f &n ) { *(reinterpret_cast<Vec3f*>( &mPtr[mNormalOffset] )) = n; }
		//! Assigns a new RGB color to the current vertex
		void	setColorRGB( const Color &n ) { *(reinterpret_cast<Color*>( &mPtr[mColorRGBOffset] )) = n; }
		//! Assigns a new RGBA color to the current vertex
		void	setColorRGBA( const ColorA &n ) { *(reinterpret_cast<ColorA*>( &mPtr[mColorRGBAOffset] )) = n; }
		//! Assigns a new 2d texture coordinate on the first texture unit to the current vertex
		void	setTexCoord2d0( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[0]] )) = t; }
		//! Assigns a new 3d texture coordinate on the first texture unit to the current vertex
		void	setTexCoord3d0( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[0]] )) = t; }
		//! Assigns a new 2d texture coordinate on the second texture unit to the current vertex
		void	setTexCoord2d1( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[1]] )) = t; }
		//! Assigns a new 3d texture coordinate on the second texture unit to the current vertex
		void	setTexCoord3d1( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[1]] )) = t; }
		//! Assigns a new 2d texture coordinate on the third texture unit to the current vertex
		void	setTexCoord2d2( const Vec2f &t ) { *(reinterpret_cast<Vec2f*>( &mPtr[mTexCoordOffset[2]] )) = t; }
		//! Assigns a new 3d texture coordinate on the third texture unit to the current vertex
		void	setTexCoord3d2( const Vec3f &t ) { *(reinterpret_cast<Vec3f*>( &mPtr[mTexCoordOffset[2]] )) = t; }
		//! Assigns a new floating point value as a custom attribute to the current vertex
		void	setCustomFloat( size_t index, float v ) { *(reinterpret_cast<float*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		//! Assigns a new 2d vector as a custom attribute to the current vertex
		void	setCustomVec2f( size_t index, const Vec2f &v ) { *(reinterpret_cast<Vec2f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		//! Assigns a new 3d vector as a custom attribute to the current vertex
		void	setCustomVec3f( size_t index, const Vec3f &v ) { *(reinterpret_cast<Vec3f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }
		//! Assigns a new 4d vector as a custom attribute to the current vertex
		void	setCustomVec4f( size_t index, const Vec4f &v ) { *(reinterpret_cast<Vec4f*>( &mPtr[mObj->mCustomOffsets[index]] )) = v; }

		//! Advances buffer pointer to the next vertex
		void 	operator++() { mPtr += mStride; }
		//! Returns true if the iterator may be advanced
		bool	isDone() const { return mPtr < mDataEnd; }
		
		//! Returns which vertex the iterator is pointing to
		size_t	getIndex() const { return ( mPtr - mData ) / mStride; }
		//! Returns which vertex the iterator is pointing to
		size_t	getStride() const { return mStride; }
		//! Returns raw pointer where the iterator is currently writing
		void*	getPointer() const { return reinterpret_cast<void*>( mPtr ); }
		//! Returns pointer where the iterator is currently writing positions
		Vec3f*	getPositionPointer() const { return reinterpret_cast<Vec3f*>( &mPtr[mPositionOffset] ); }		

//		VertexIter( const VertexIter &other ) { set( other ); }	
//		VertexIter& operator=( const VertexIter &other ) { set( other ); return *this; }
		
	 protected:
		void set( const VertexIter &other );

		/** Internal shared implementation for the vertex buffer iterator type */
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
