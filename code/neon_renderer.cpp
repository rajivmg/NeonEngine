#include "neon_renderer.h"

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
////
////	Shader
////
shader::shader()
{
	ProgramAvailable = false;
}

shader::~shader()
{

}

void shader::CreateProgram(read_file_result *VsFile, read_file_result *FsFile)
{
	Vs = glCreateShader(GL_VERTEX_SHADER);
	Fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(Vs, 1, (GLchar * const *)(&VsFile->Content), (const GLint *)(&VsFile->ContentSize));
	glShaderSource(Fs, 1, (GLchar * const *)(&FsFile->Content), (const GLint *)(&FsFile->ContentSize));

	glCompileShader(Vs);
	glCompileShader(Fs);

	Program = glCreateProgram();
	glAttachShader(Program, Vs);
	glAttachShader(Program, Fs);

	glLinkProgram(Program);
		
	glValidateProgram(Program);
	GLint Validated;
	glGetProgramiv(Program, GL_VALIDATE_STATUS, &Validated);
	
	if(!Validated)
	{
		char VsErrors[8192];
		char FsErrors[8192];
		char ProgramErrors[8192];
		glGetShaderInfoLog(Vs, sizeof(VsErrors), 0, VsErrors);
		glGetShaderInfoLog(Fs, sizeof(FsErrors), 0, FsErrors);
		glGetProgramInfoLog(Program, sizeof(ProgramErrors), 0, ProgramErrors);

		Assert(!"Shader compilation and/or linking failed");	
	}	

	ProgramAvailable = true;

	glDetachShader(Program, Vs);
	glDetachShader(Program, Fs);

	glDeleteShader(Vs);
	glDeleteShader(Fs);
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////
////	Quad related functions
////
texture_quad TextureQuad(vec2 Origin, vec2 Size, vec4 Color , vec4 UVCoords)
{
	texture_quad QuadVertex;
/*
	D--------C
	|  U 	/|
	|      / |
	|  	  /	 |
	|    /   |
	|	/    |
	|  /     |
	| /      |
	|/    B  |
	A--------B
	
	A.XYZ = Origin.XYZ
	A.UV  = UVCoords.XY

	B.X   = Origin.X + Size.X - 1
	B.Y   = Origin.Y
	B.Z   = 0
	B.U   = UVCoords.Z 
	B.V   = UVCoords.Y 

	C.X   = Origin.X + Size.X - 1
	C.Y   = Origin.Y + Size.Y -1
	C.Z   = 0
	C.UV  = UVCoords.ZW

	D.X   = Origin.X 
	D.Y   = Origin.Y + Size.Y - 1
	D.Z   = 0
	D.U   = UVCoords.X
	D.V   = UVCoords.W
 */

	// Upper triangle
	// D
	QuadVertex.Content[0] = Origin.X;
	QuadVertex.Content[1] = Origin.Y + Size.Y;
	QuadVertex.Content[2] = 0;

	QuadVertex.Content[3]  = Color.R;
	QuadVertex.Content[4]  = Color.G;
	QuadVertex.Content[5]  = Color.B;
	QuadVertex.Content[6]  = Color.A;

	QuadVertex.Content[7]  = UVCoords.X;
	QuadVertex.Content[8]  = UVCoords.W;

	// C
	QuadVertex.Content[9]  = Origin.X + Size.X;
	QuadVertex.Content[10] = Origin.Y + Size.Y;
	QuadVertex.Content[11] = 0;

	QuadVertex.Content[12] = Color.R;
	QuadVertex.Content[13] = Color.G;
	QuadVertex.Content[14] = Color.B;
	QuadVertex.Content[15] = Color.A;

	QuadVertex.Content[16] = UVCoords.Z;
	QuadVertex.Content[17] = UVCoords.W;

	// A
	QuadVertex.Content[18] = Origin.X;
	QuadVertex.Content[19] = Origin.Y;
	QuadVertex.Content[20] = 0;

	QuadVertex.Content[21] = Color.R;
	QuadVertex.Content[22] = Color.G;
	QuadVertex.Content[23] = Color.B;
	QuadVertex.Content[24] = Color.A;
  
	QuadVertex.Content[25] = UVCoords.X;
	QuadVertex.Content[26] = UVCoords.Y;


	// Lower triangle
	// A
	QuadVertex.Content[27] = Origin.X;
	QuadVertex.Content[28] = Origin.Y;
	QuadVertex.Content[29] = 0;

	QuadVertex.Content[30] = Color.R;
	QuadVertex.Content[31] = Color.G;
	QuadVertex.Content[32] = Color.B;
	QuadVertex.Content[33] = Color.A;
  
	QuadVertex.Content[34] = UVCoords.X;
	QuadVertex.Content[35] = UVCoords.Y;

	// C
	QuadVertex.Content[36] = Origin.X + Size.X;
	QuadVertex.Content[37] = Origin.Y + Size.Y;
	QuadVertex.Content[38] = 0;

	QuadVertex.Content[39] = Color.R;
	QuadVertex.Content[40] = Color.G;
	QuadVertex.Content[41] = Color.B;
	QuadVertex.Content[42] = Color.A;

	QuadVertex.Content[43] = UVCoords.Z;
	QuadVertex.Content[44] = UVCoords.W;

	// B
	QuadVertex.Content[45] = Origin.X + Size.X;
	QuadVertex.Content[46] = Origin.Y;
	QuadVertex.Content[47] = 0;

	QuadVertex.Content[48] = Color.R;
	QuadVertex.Content[49] = Color.G;
	QuadVertex.Content[50] = Color.B;
	QuadVertex.Content[51] = Color.A;

	QuadVertex.Content[52] = UVCoords.Z;
	QuadVertex.Content[53] = UVCoords.Y;

	return QuadVertex;
}

color_quad ColorQuad(vec2 Origin, vec2 Size, vec4 Color)
{

	color_quad QuadVertex;
	// Upper triangle
	// D
	QuadVertex.Content[0] = Origin.X;
	QuadVertex.Content[1] = Origin.Y + Size.Y;
	QuadVertex.Content[2] = 0;

	QuadVertex.Content[3]  = Color.R;
	QuadVertex.Content[4]  = Color.G;
	QuadVertex.Content[5]  = Color.B;
	QuadVertex.Content[6]  = Color.A;

	// C
	QuadVertex.Content[7]  = Origin.X + Size.X;
	QuadVertex.Content[8] = Origin.Y + Size.Y;
	QuadVertex.Content[9] = 0;

	QuadVertex.Content[10] = Color.R;
	QuadVertex.Content[11] = Color.G;
	QuadVertex.Content[12] = Color.B;
	QuadVertex.Content[13] = Color.A;

	// A
	QuadVertex.Content[14] = Origin.X;
	QuadVertex.Content[15] = Origin.Y;
	QuadVertex.Content[16] = 0;

	QuadVertex.Content[17] = Color.R;
	QuadVertex.Content[18] = Color.G;
	QuadVertex.Content[19] = Color.B;
	QuadVertex.Content[20] = Color.A;

	// Lower t.angle
	// A
	QuadVertex.Content[21] = Origin.X;
	QuadVertex.Content[22] = Origin.Y;
	QuadVertex.Content[23] = 0;

	QuadVertex.Content[24] = Color.R;
	QuadVertex.Content[25] = Color.G;
	QuadVertex.Content[26] = Color.B;
	QuadVertex.Content[27] = Color.A;

	// C
	QuadVertex.Content[28] = Origin.X + Size.X;
	QuadVertex.Content[29] = Origin.Y + Size.Y;
	QuadVertex.Content[30] = 0;

	QuadVertex.Content[31] = Color.R;
	QuadVertex.Content[32] = Color.G;
	QuadVertex.Content[33] = Color.B;
	QuadVertex.Content[34] = Color.A;

	// B
	QuadVertex.Content[35] = Origin.X + Size.X;
	QuadVertex.Content[36] = Origin.Y;
	QuadVertex.Content[37] = 0;

	QuadVertex.Content[38] = Color.R;
	QuadVertex.Content[39] = Color.G;
	QuadVertex.Content[40] = Color.B;
	QuadVertex.Content[41] = Color.A;

	return QuadVertex;
}

quad_batch::quad_batch()
{
	QuadCount = 0;
	TextureUnit = 0;
	Initialised = false;
	GPUBufferAvailable = false;
	ShaderAvailable = false;
	TextureSet = false;
	FlushAfterDraw = true;
}

quad_batch::~quad_batch()
{
}

void quad_batch::Init(quad_type aQuadType, u32 BufferSize, bool AutoFlush)
{
	QuadType 			= aQuadType;
	Buffer.MemSize 	   	= BufferSize;
	Buffer.MemAvailable	= BufferSize; // In bytes
	Buffer.MemUsed 	   	= 0;
	Buffer.Content 	   	= malloc(BufferSize);
	Buffer.Head			= Buffer.Content;

	if(Buffer.Content == 0)
	{
		Assert(!"Memory allocation error");
	}

	Initialised = true;
	FlushAfterDraw = AutoFlush;
}

void quad_batch::SetTexture(texture *Texture)
{
	Assert(QuadType == TEXTURE_QUAD);

	// glTexImage2D expect the texture data in the bottom-up row fashion.
	Texture->FlipVertically(); 

	TextureUnit = TextureManager(GET_TEXTURE_UNIT);
	glActiveTexture(GL_TEXTURE0 + TextureUnit);

	glGenTextures(1, &TEX);
	glBindTexture(GL_TEXTURE_2D, TEX);
	
	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGBA,
				 Texture->Width,
				 Texture->Height,
				 0,
				 GL_RGBA,
				 GL_UNSIGNED_BYTE,
				 Texture->Content);

	//@TODO: needs testing
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	TextureSet = true;
}

void quad_batch::SetShader(shader *aShader)
{
	Shader = aShader;
	ShaderAvailable = true;
}

void quad_batch::PushQuad(texture_quad *Quad)
{
	Assert(QuadType == TEXTURE_QUAD);

	if(Buffer.Content == 0)
	{
		this->Init(TEXTURE_QUAD, KILOBYTE(64));
	}

	if(Buffer.MemAvailable >= sizeof(texture_quad))
	{
		memcpy(Buffer.Content, Quad->Content, sizeof(texture_quad));
		Buffer.Content = (texture_quad *)Buffer.Content + 1; // Move the pointer forward to point to unused memory
		Buffer.MemAvailable -= sizeof(texture_quad);
		Buffer.MemUsed += sizeof(texture_quad);
		++QuadCount;
	}
	else
	{
		Assert(!"Quad buffer memory full.");
	}
}

void quad_batch::PushQuad(color_quad *Quad)
{
	Assert(QuadType == COLOR_QUAD);

	if(Buffer.Content == 0)
	{
		this->Init(COLOR_QUAD, KILOBYTE(64));
	}

	if(Buffer.MemAvailable >= sizeof(color_quad))
	{
		memcpy(Buffer.Content, Quad->Content, sizeof(color_quad));
		Buffer.Content = (color_quad *)Buffer.Content + 1; // Move the pointer forward to point to unused memory
		Buffer.MemAvailable -= sizeof(color_quad);
		Buffer.MemUsed += sizeof(color_quad);
		++QuadCount;
	}
	else
	{
		Assert(!"Quad buffer memory full.");
	}
}

void quad_batch::CreateGPUBuffer()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, Buffer.MemSize, Buffer.Head, GL_DYNAMIC_DRAW);
	
	glGenVertexArrays(1, &VAO);
	
	glBindVertexArray(VAO);
	
	Assert(ShaderAvailable);
	
	GLint posLoc = glGetAttribLocation(Shader->Program, "in_position");
	Assert(posLoc != -1);
	GLint colorLoc = glGetAttribLocation(Shader->Program, "in_color");
	Assert(colorLoc != -1);

	if(QuadType == TEXTURE_QUAD)
	{
		GLint texcoordLoc = glGetAttribLocation(Shader->Program, "in_texcoord");
		Assert(texcoordLoc != -1);

		glVertexAttribPointer((GLuint)posLoc, 3, GL_FLOAT, GL_FALSE, 36, (GLvoid *)0);
		glEnableVertexAttribArray((GLuint)posLoc);
		
		glVertexAttribPointer((GLuint)colorLoc, 4, GL_FLOAT, GL_FALSE, 36, (GLvoid *)12);
		glEnableVertexAttribArray((GLuint)colorLoc);
		
		glVertexAttribPointer((GLuint)texcoordLoc, 2, GL_FLOAT, GL_FALSE, 36, (GLvoid *)28);
		glEnableVertexAttribArray((GLuint)texcoordLoc);
	
		GLint posSampler = glGetUniformLocation(Shader->Program, "map0");
		Assert(posSampler != -1);

		glUseProgram(Shader->Program);
		glUniform1i(posSampler, TextureUnit);
	}
	else if(QuadType == COLOR_QUAD)
	{
		glVertexAttribPointer((GLuint)posLoc, 3, GL_FLOAT, GL_FALSE, 28, (GLvoid *)0);
		glEnableVertexAttribArray((GLuint)posLoc);
		
		glVertexAttribPointer((GLuint)colorLoc, 4, GL_FLOAT, GL_FALSE, 28, (GLvoid *)12);
		glEnableVertexAttribArray((GLuint)colorLoc);
	}
	GPUBufferAvailable = true;
}

void quad_batch::UpdateGPUBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, Buffer.MemUsed, Buffer.Head);
}

void quad_batch::Flush()
{
	QuadCount 		= 0;
	Buffer.Content 	= Buffer.Head;
	Buffer.MemUsed 	= 0;
	Buffer.MemAvailable = Buffer.MemSize;
}

void quad_batch::Draw()
{
	if(QuadType != COLOR_QUAD)
	{
		Assert(TextureSet == true);
	}

	if(!GPUBufferAvailable)
	{
		this->CreateGPUBuffer();
	}
	else
	{
		this->UpdateGPUBuffer();
	}
	
	glBindVertexArray(VAO);
	glUseProgram(Shader->Program);
	
	mat4 Proj = OrthoMat4(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);

	GLint projLoc = glGetUniformLocation(Shader->Program, "proj");
	Assert(projLoc != -1);
	glUniformMatrix4fv((GLuint)projLoc, 1, GL_TRUE, Proj.Elements);
	
	GLsizei ContentSize = (QuadType == TEXTURE_QUAD) ? sizeof(texture_quad) : sizeof(color_quad);
	glDrawArrays(GL_TRIANGLES, 0, Buffer.MemUsed / ContentSize * 6);

	if(FlushAfterDraw == true)
	{
		this->Flush();
	}
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
//// 	Line batch
////
line_batch::line_batch()
{
	LineCount = 0;
	Initialised = false;
	GPUBufferAvailable = false;
	ShaderAvailable = false;
	FlushAfterDraw = true;
}

line_batch::~line_batch()
{
}

void line_batch::Init(u32 BufferSize, bool AutoFlush)
{
	Buffer.MemSize 	   	= BufferSize;
	Buffer.MemAvailable	= BufferSize; // In bytes
	Buffer.MemUsed 	   	= 0;
	Buffer.Content 	   	= malloc(BufferSize);
	Buffer.Head			= Buffer.Content;

	if(Buffer.Content == 0)
	{
		Assert(!"Memory allocation error");
	}

	Initialised = true;
	FlushAfterDraw = AutoFlush;
}

void line_batch::SetShader(shader *aShader)
{
	Shader = aShader;
	ShaderAvailable = true;
}

void line_batch::PushLine(vec3 Start, vec3 End, vec4 Color)
{
	if(Buffer.Content == 0)
	{
		this->Init(KILOBYTE(64));
	}

	if(Buffer.MemAvailable >= sizeof(line_3d))
	{
		// create line
		line_3d Line;
		Line.Content[0] = Start.X + 0.5f; //@NOTE: Half pixel correction
		Line.Content[1] = Start.Y + 0.5f;
		Line.Content[2] = Start.Z;

		Line.Content[3] = Color.R;
		Line.Content[4] = Color.G;
		Line.Content[5] = Color.B;
		Line.Content[6] = Color.A;

		Line.Content[7] = End.X + 0.5f;
		Line.Content[8] = End.Y + 0.5f;
		Line.Content[9] = End.Z;

		Line.Content[10] = Color.R;
		Line.Content[11] = Color.G;
		Line.Content[12] = Color.B;
		Line.Content[13] = Color.A;

		memcpy(Buffer.Content, Line.Content, sizeof(line_3d));
		Buffer.Content = (line_3d *)Buffer.Content + 1; // Move the pointer forward to point to unused memory
		Buffer.MemAvailable -= sizeof(line_3d);
		Buffer.MemUsed += sizeof(line_3d);
		++LineCount;
	}
	else
	{
		Assert(!"Line buffer memory full.");
	}
}

void line_batch::PushLine(vec3 Start, r32 Radian, u32 Length, vec4 Color)
{
	if(Buffer.Content == 0)
	{
		this->Init(KILOBYTE(64));
	}

	if(Buffer.MemAvailable >= sizeof(line_3d))
	{
		// create line
		line_3d Line;
		Line.Content[0] = Start.X + 0.5f; //@NOTE: Half pixel correction
		Line.Content[1] = Start.Y + 0.5f;
		Line.Content[2] = Start.Z;

		Line.Content[3] = Color.R;
		Line.Content[4] = Color.G;
		Line.Content[5] = Color.B;
		Line.Content[6] = Color.A;

		Line.Content[7] = (GLfloat) ( Start.X + Length * cos(Radian) ); //End.X + 0.5f;
		Line.Content[8] = (GLfloat) ( Start.Y + Length * sin(Radian) ); //End.Y + 0.5f;
		Line.Content[9] = Start.Z;

		Line.Content[10] = Color.R;
		Line.Content[11] = Color.G;
		Line.Content[12] = Color.B;
		Line.Content[13] = Color.A;

		// vec3 Start, End;
		// Start.X = 0.0f;
		// Start.Y = Platform->Height;
		// Start.Z = 0.0f;
	
		// End.X = Start.X + 500 * cos((3.14/180) * 315);
		// End.Y = Start.Y + 500 * sin((3.14/180) * 315);
		// End.Z = 0.0f;

		memcpy(Buffer.Content, Line.Content, sizeof(line_3d));
		Buffer.Content = (line_3d *)Buffer.Content + 1; // Move the pointer forward to point to unused memory
		Buffer.MemAvailable -= sizeof(line_3d);
		Buffer.MemUsed += sizeof(line_3d);
		++LineCount;
	}
	else
	{
		Assert(!"Line buffer memory full.");
	}
}

void line_batch::CreateGPUBuffer()
{
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, Buffer.MemSize, Buffer.Head, GL_DYNAMIC_DRAW);
	
	glGenVertexArrays(1, &VAO);
	
	glBindVertexArray(VAO);
	
	Assert(ShaderAvailable);
	
	GLint posLoc = glGetAttribLocation(Shader->Program, "in_position");
	Assert(posLoc != -1);
	GLint colorLoc = glGetAttribLocation(Shader->Program, "in_color");
	Assert(colorLoc != -1);

	glVertexAttribPointer((GLuint)posLoc, 3, GL_FLOAT, GL_FALSE, 28, (GLvoid *)0);
	glEnableVertexAttribArray((GLuint)posLoc);
	
	glVertexAttribPointer((GLuint)colorLoc, 4, GL_FLOAT, GL_FALSE, 28, (GLvoid *)12);
	glEnableVertexAttribArray((GLuint)colorLoc);

	GPUBufferAvailable = true;
}

void line_batch::UpdateGPUBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, Buffer.MemUsed, Buffer.Head);
}

void line_batch::Flush()
{
	LineCount 		= 0;
	Buffer.Content 	= Buffer.Head;
	Buffer.MemUsed 	= 0;
	Buffer.MemAvailable = Buffer.MemSize;
}

void line_batch::Draw()
{
	if(!GPUBufferAvailable)
	{
		this->CreateGPUBuffer();
	}
	else
	{
		this->UpdateGPUBuffer();
	}
	
	glBindVertexArray(VAO);
	glUseProgram(Shader->Program);
	
	mat4 Proj = OrthoMat4(0.0f, (r32)Platform->Width, (r32)Platform->Height, 0.0f, -1.0f, 1.0f);

	GLint projLoc = glGetUniformLocation(Shader->Program, "proj");
	Assert(projLoc != -1);
	glUniformMatrix4fv((GLuint)projLoc, 1, GL_TRUE, Proj.Elements);
	
	GLsizei ContentSize = sizeof(line_3d);
	glDrawArrays(GL_LINES, 0, Buffer.MemUsed / ContentSize * 2); // change to LineCount * 2

	if(FlushAfterDraw == true)
	{
		this->Flush();
	}
}


//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Texture manager functions
////
u16 TextureManager(tex_command Command, u16 TexUnit)
{
#define MAX_TEX_UNITS 12
	local_persist b32 Unit[12];
	//@NOTE: Currently max texture count is 12.
	// Unit[] is initialised to 0. 0 means free and 1 means in use.
	if(Command == GET_TEXTURE_UNIT)
	{
		for(u16 i = 0; i < MAX_TEX_UNITS; ++i)
		{
			if(Unit[i] == 0)
			{
				Unit[i] = 1;
				return i;
			}
		}
		Assert(!"No free texture unit")
	}
	else if(Command == RELEASE_TEXTURE_UNIT)
	{
		Assert(TexUnit != -1);
		Assert(Unit[TexUnit] != 0); // Assert if the TextureUnit is already free.
		Unit[TexUnit] = 0;
	}
	return (u16)-1;
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Text rendering functions
////
font::font()
{
	Initialised = false;
}

font::~font()
{
}

void font::Load(char const * FontSrc, u16 aFontHeight)
{
	FontHeight = aFontHeight;
	Glyphs = (glyph *)malloc(sizeof(glyph) * (128-32));
	Atlas.Initialise(512, 512, 2); // padding = 2

	FT_Library FTLib;
	FT_Face Face;

	int Error = FT_Init_FreeType(&FTLib);
	if(Error)
	{
		Assert(!"Error initialising the FreeType library.");
	}

	read_file_result FontData = Platform->ReadFile(FontSrc);

	Error = FT_New_Memory_Face(FTLib, (const FT_Byte *)FontData.Content, FontData.ContentSize, 0, &Face);
	if(Error == FT_Err_Unknown_File_Format)
	{
		Assert(!"File format is not supported.");
	}
	else if(Error)
	{
		Assert(!"File could not be opened or read.");
	}

	Error = FT_Set_Pixel_Sizes(Face, 0, FontHeight);
	Assert(Error == 0);

	texture *GlyphTexture = (texture *)malloc(sizeof(texture));
	for(int CIndex = 32; CIndex < 128; ++CIndex)
	{
		int GlyphIndex = FT_Get_Char_Index(Face, CIndex);
	
		Error = FT_Load_Glyph(Face, GlyphIndex, FT_LOAD_DEFAULT);
		Assert(Error == 0);
		
		if(Face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
		{
			FT_Render_Glyph(Face->glyph, FT_RENDER_MODE_NORMAL);
		}

		glyph *Glyph = Glyphs + (CIndex - 32);
		Glyph->Width = Face->glyph->bitmap.width;
		Glyph->Height = Face->glyph->bitmap.rows;
		
		u8 *GlyphBitmap = Face->glyph->bitmap.buffer;

		FT_Glyph_Metrics *Metrics = &Face->glyph->metrics;

		Glyph->HoriBearingX = Metrics->horiBearingX/64;
		Glyph->HoriBearingY = Metrics->horiBearingY/64;
		Glyph->HoriAdvance  = Metrics->horiAdvance/64;
		Glyph->Hang 		= Glyph->HoriBearingY - Glyph->Height;
		
		GlyphTexture->Width = Glyph->Width;
		GlyphTexture->Height = Glyph->Height;
		GlyphTexture->ContentSize = GlyphTexture->Width * GlyphTexture->Height * 4;
		if(GlyphTexture->ContentSize != 0)
		{
			GlyphTexture->Content = malloc(GlyphTexture->ContentSize);
			Assert(GlyphTexture != 0);
			memset(GlyphTexture->Content, 255, GlyphTexture->ContentSize);
		}
		
		u32 *DestTexel = (u32 *)GlyphTexture->Content;
		u8 *SrcTexel = (u8 *)GlyphBitmap;

		// convert Glpyh's 8bit texture to 32bit texture.
		for(int x = 0; x < GlyphTexture->Width * GlyphTexture->Height; ++x)
		{
			u8 *DestGreen = (u8 *)DestTexel+3;
			*DestGreen = *SrcTexel;

			DestTexel++;
			SrcTexel++;
		}

		if(GlyphTexture->ContentSize != 0)
		{
			// char Temp[20];
			// snprintf(Temp, 20, "%d.tga", CIndex);
			// DebugTextureSave(Temp, GlyphTexture);

			Glyph->Coordinates = Atlas.PackTexture(GlyphTexture);
		}

		// After glyph texture has been copied to texture atlas free
		// glyph texture memory.
		if(GlyphTexture->ContentSize != 0)
		{
			free(GlyphTexture->Content);
		}
	}

	// debugging purpose
	// texture AtlasDebugCopy;
	// AtlasDebugCopy = Atlas.ToTexture();
	// DebugTextureSave("FontAtlas.tga", &AtlasDebugCopy);
	
	Initialised = true;
}

void font::FreeMemory()
{
	Assert(Initialised);
	SAFE_FREE(Glyphs);
	Atlas.FreeMemory();
}

text_batch::text_batch()
{
	FontSet = false;
}

text_batch::~text_batch()
{
}

void text_batch::SetFont(font *aFont, u32 BufferSize)
{
	Assert(aFont->Initialised);
	Font = aFont;
	FontSet = true;

	// initialise the quad_batch.
	Batch.Init(TEXTURE_QUAD, BufferSize, false);

	Batch.SetShader(RenderResources.TextShader);

	texture FontTexture = Font->Atlas.ToTexture();

	// since we are calculating the texture coordinates by considering (0,0)
	// as the top left of the texture, but opengl expect the texture
	// coordinates (0, 0) as bottom left of the texture. we need to flip the
	// texture once.
	FontTexture.FlipVertically();

	Batch.SetTexture(&FontTexture);
}

void text_batch::PushText(vec2 Origin, vec4 Color, char const *Fmt, ...)
{
	char Text[8192];

	va_list Arguments;
	va_start(Arguments, Fmt);

	vsnprintf(Text, 8192, Fmt, Arguments);

	va_end(Arguments);

	Assert(FontSet);

	Pen = Origin;

	int Index = 0;
	while(Text[Index] != 0)
	{
		if((int)Text[Index] == 10)
		{
			Pen.X = Origin.X;
			Pen.Y -= Font->FontHeight;
			Index++;
			continue;
		}

		glyph *CharGlyph = Font->Glyphs + ((int)Text[Index] - 32);
		vec4 TexCoords = vec4(CharGlyph->Coordinates.BL_X, CharGlyph->Coordinates.BL_Y, CharGlyph->Coordinates.TR_X, CharGlyph->Coordinates.TR_Y);
		
		vec2 CharOrigin = vec2(Pen.X + CharGlyph->HoriBearingX, Pen.Y + CharGlyph->Hang);

		texture_quad CharQuad = TextureQuad(CharOrigin, vec2(CharGlyph->Width, CharGlyph->Height), Color, TexCoords);
		Batch.PushQuad(&CharQuad);

		Pen.X += CharGlyph->HoriAdvance; 

		Index++;
	}
}

void text_batch::Draw()
{
	Batch.Draw();
}

void text_batch::Flush()
{
	Batch.Flush();
}

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
////
////	Renderer functions
////
void InitRenderResources()
{
	read_file_result VertShdrFile, FragShdrFile;

	// texture quad shader
	VertShdrFile = Platform->ReadFile("texture_quad.vs");
	FragShdrFile = Platform->ReadFile("texture_quad.fs");
	
	RenderResources.TQuadShader = (shader *)malloc(sizeof(shader));
	RenderResources.TQuadShader->CreateProgram(&VertShdrFile, &FragShdrFile);

	Platform->FreeFileMemory(&VertShdrFile);
	Platform->FreeFileMemory(&FragShdrFile);


	// color quad shader
	VertShdrFile = Platform->ReadFile("color_quad.vs");
	FragShdrFile = Platform->ReadFile("color_quad.fs");
	
	RenderResources.CQuadShader = (shader *)malloc(sizeof(shader));
	RenderResources.CQuadShader->CreateProgram(&VertShdrFile, &FragShdrFile);

	Platform->FreeFileMemory(&VertShdrFile);
	Platform->FreeFileMemory(&FragShdrFile);

	// text shader

	VertShdrFile = Platform->ReadFile("text.vs");
	FragShdrFile = Platform->ReadFile("text.fs");

	RenderResources.TextShader = (shader *)malloc(sizeof(shader));
	RenderResources.TextShader->CreateProgram(&VertShdrFile, &FragShdrFile);

	Platform->FreeFileMemory(&VertShdrFile);
	Platform->FreeFileMemory(&FragShdrFile);

	// HUD font
	RenderResources.HUDFont = (font *)malloc(sizeof(font));
	RenderResources.HUDFont->Load("fonts\\Inconsolata-Regular.ttf", 28);
}

void InitRenderer()
{
	// initialise opengl function pointers.
	InitGL();

	// initialise render resources
	InitRenderResources();

	// clear the color to blue.
	glClearColor(0.13f, 0.18f, 0.48f, 1.0f);

	// set the viewport
	glViewport(0, 0, Platform->Width, Platform->Height);

	// since we are following the left-handed coodinate system we set
	// clock-wise vertex winding as the triangle's front.
	glFrontFace(GL_CW);

	// enable back-face culling to improve performance.
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// enable the blending to support alpha textures. 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ClearBackBuffer()
{
	// clear the back-buffer's color and depth bits.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}