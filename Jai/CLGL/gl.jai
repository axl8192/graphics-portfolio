#scope_file
#import "Basic";
#import "Windows";

#scope_export
old :: #import "GL";

//### Vertex Array Objects ###//
VertexArray :: #type,distinct u32;

createVertexArray :: inline () -> VertexArray{
    va: VertexArray;
    old.glCreateVertexArrays(1, xx *va);
    return va;
}

deleteVertexArray :: inline (vaobj: VertexArray){
    old.glDeleteVertexArrays(1, xx *vaobj);
}

bindVertexArray :: inline (vertex_array: VertexArray){
    old.glBindVertexArray(xx vertex_array);
}

vertexArrayVertexBuffer :: inline (vaobj: VertexArray, bindingindex: u32, buffer: Buffer, offset: s64, stride: u32){
    old.glVertexArrayVertexBuffer(xx vaobj, bindingindex, xx buffer, offset, stride);
}

enableVertexArrayAttrib :: inline (vaobj: VertexArray, index: u32){
    old.glEnableVertexArrayAttrib(xx vaobj, index);
}

VertexArrayAttribFormatType :: enum {
    S8  :: old.GL_BYTE;
    S16 :: old.GL_SHORT;
    S32 :: old.GL_INT;
    
    F16 :: old.GL_HALF_FLOAT;
    F32 :: old.GL_FLOAT;
    F64 :: old.GL_DOUBLE;
    
    U8  :: old.GL_UNSIGNED_BYTE;
    U16 :: old.GL_UNSIGNED_SHORT;
    U32 :: old.GL_UNSIGNED_INT;

    //@@incomplete
    //old.GL_FIXED;
    //old.GL_INT_2_10_10_10_REV;
    //old.GL_UNSIGNED_INT_2_10_10_10_REV;
    //old.GL_UNSIGNED_INT_10F_11F_11F_REV;
}

vertexArrayAttribFormat :: inline (vaobj: VertexArray, attribindex: u32, size: s32, type: VertexArrayAttribFormatType, relativeoffset: u32, normalized: bool = false){
    old.glVertexArrayAttribFormat(xx vaobj, attribindex, size, xx type, xx normalized, relativeoffset);
}

vertexArrayAttribBinding :: inline (vaobj: VertexArray, attribindex: u32, bindingindex: u32){
    old.glVertexArrayAttribBinding(xx vaobj, attribindex, bindingindex);
}

vertexArrayElementBuffer :: inline (vaobj: VertexArray, buffer: Buffer){
    old.glVertexArrayElementBuffer(xx vaobj, xx buffer);
}

//### Buffer Objects ###//
Buffer :: #type,distinct u32;

createBuffer :: inline () -> Buffer{
    buff: Buffer;
    old.glCreateBuffers(1, xx *buff);
    return buff;
}

deleteBuffer :: inline (buffer: Buffer){
    old.glDeleteBuffers(1, xx *buffer);
}

BufferStorageFlags :: enum_flags {
    NONE :: old.GL_NONE;
    DYNAMIC_STORAGE :: old.GL_DYNAMIC_STORAGE_BIT;
    MAP_READ :: old.GL_MAP_READ_BIT;
    MAP_WRITE :: old.GL_MAP_WRITE_BIT;
    MAP_PERSISTENT :: old.GL_MAP_PERSISTENT_BIT;
    MAP_COHERENT :: old.GL_MAP_COHERENT_BIT;
    CLIENT_STORAGE :: old.GL_CLIENT_STORAGE_BIT;
}

bufferStorage :: inline (buffer: Buffer, array: [] $T, flags: BufferStorageFlags = .NONE){
    old.glNamedBufferStorage(xx buffer, array.count*size_of(T), array.data, xx flags);
}

bufferStorage :: inline (buffer: Buffer, size: s64, data: *void, flags: BufferStorageFlags = .NONE){ // @@@@Change s64 for u32 for correctness? [or not, for comfort]
    old.glNamedBufferStorage(xx buffer, xx size, data, xx flags);
}

bufferSubData :: (buffer: Buffer, offset: s64, size: s64, data: *void){
    old.glNamedBufferSubData(xx buffer, xx offset, xx size, data);
}

BindBufferBaseTargets :: enum{
    SHADER_STORAGE_BUFFER :: old.GL_SHADER_STORAGE_BUFFER;
    UNIFORM_BUFFER :: old.GL_UNIFORM_BUFFER;
    TRANSFORM_FEEDBACK_BUFFER :: old.GL_TRANSFORM_FEEDBACK_BUFFER;
    ATOMIC_COUNTER_BUFFER :: old.GL_ATOMIC_COUNTER_BUFFER; 
}

bindBufferBase :: (target: BindBufferBaseTargets, index: u32, buffer: Buffer){
    old.glBindBufferBase(xx target, index, xx buffer);
}

//### Shaders ###//
Shader :: #type,distinct u32;

ShaderType :: enum {
    VERTEX :: old.GL_VERTEX_SHADER;
    FRAGMENT :: old.GL_FRAGMENT_SHADER;
    GEOMETRY :: old.GL_GEOMETRY_SHADER;
    COMPUTE :: old.GL_COMPUTE_SHADER;
    
    TESSELATION_CONTROL :: old.GL_TESS_CONTROL_SHADER; 
    TESSELATION_EVALUATION :: old.GL_TESS_EVALUATION_SHADER;
}

createShader :: inline (type: ShaderType) -> Shader{
    shader: Shader;
    shader = xx old.glCreateShader(xx type);
    return shader;
}

deleteShader :: inline (shader: Shader){
    old.glDeleteShader(xx shader);
}

shaderSource :: inline (shader: Shader, source : string){
    old.glShaderSource(xx shader, 1, xx *source.data, xx *source.count);
}

compileShader :: inline (shader: Shader) {
    old.glCompileShader(xx shader);
}

// The next functions are, together, basically glGetShaderiv :: 
getShaderType :: inline (shader: Shader) -> ShaderType{
    result: s32;
    old.glGetShaderiv(xx shader, old.GL_SHADER_TYPE, *result);
    return xx result;
}
getShaderDeleteStatus :: inline (shader: Shader) -> bool{
    result: s32;
    old.glGetShaderiv(xx shader, old.GL_DELETE_STATUS, *result);
    return xx result;
}
getShaderCompileStatus :: inline (shader: Shader) -> bool{
    result: s32;
    old.glGetShaderiv(xx shader, old.GL_COMPILE_STATUS, *result);
    return xx result;
}
getShaderInfoLogLength :: inline (shader: Shader) -> s32{
    result: s32;
    old.glGetShaderiv(xx shader, old.GL_INFO_LOG_LENGTH, *result);
    return result;
}
getShaderSourceLength :: inline (shader: Shader) -> s32{
    result: s32;
    old.glGetShaderiv(xx shader, old.GL_SHADER_SOURCE_LENGTH, *result);
    return result;
}

getShaderInfoLog :: inline (shader: Shader) -> string {
    length := getShaderInfoLogLength(shader);
    message: *u8 = xx alloc(length);
    old.glGetShaderInfoLog(xx shader, xx length, xx *length, message);
    
    str: string;
    str.data = message;
    str.count = length;
    return str;
}

//### Programs ###//
Program :: #type,distinct u32;

createProgram :: inline () -> Program {
    program: Program;
    program = xx old.glCreateProgram();
    return program;
}

deleteProgram :: inline (program: Program){
    old.glDeleteProgram(xx program);
}

bindProgram :: inline (program: Program) {
    old.glUseProgram(xx program);
}

attachShader :: inline (program: Program, shader: Shader){
    old.glAttachShader(xx program, xx shader);
}

linkProgram :: inline (program: Program) {
    old.glLinkProgram(xx program);
}

validateProgram :: inline (program: Program){
    old.glValidateProgram(xx program);
}

UniformLocation :: #type,distinct s32;

getUniformLocation :: inline (shader: Program, name: *u8) -> UniformLocation{
    return xx old.glGetUniformLocation(xx shader, name);
}

//@@@Unificar todos estos uniformSets? (TODOS o al menos agrupar algunos)
uniform1f :: inline (location: UniformLocation, v0: float32) { old.glUniform1f(xx location, v0); }
uniform2f :: inline (location: UniformLocation, v0: float32, v1: float32) { old.glUniform2f(xx location, v0, v1); }
uniform3f :: inline (location: UniformLocation, v0: float32, v1: float32, v2: float32) { old.glUniform3f(xx location, v0, v1, v2); }
uniform4f :: inline (location: UniformLocation, v0: float32, v1: float32, v2: float32, v3: float32) { old.glUniform4f(xx location, v0, v1, v2, v3); }

uniform1i :: inline (location: UniformLocation, v0: s32) { old.glUniform1i(xx location, v0); }
uniform1ui :: inline (location: UniformLocation, v0: u32) { old.glUniform1ui(xx location, v0); }

//@@@Make transpose true by default???
uniformMatrix4fv :: inline (location: UniformLocation, transpose: bool, value: *float32, count: u32 = 1) { old.glUniformMatrix4fv(xx location, count, xx transpose, value); }
uniform4fv :: inline (location: UniformLocation, value: *float32, count: u32 = 1) { old.glUniform4fv(xx location, count, value); }
uniform3fv :: inline (location: UniformLocation, value: *float32, count: u32 = 1) { old.glUniform3fv(xx location, count, value); }


//### State Management ##//
clearColor :: inline (red: float32, green: float32, blue: float32, alpha: float32){  //@jaify change the name to setClearColor?
    old.glClearColor(red, green, blue, alpha);
}

FramebufferMask :: enum_flags {
    COLOR_BIT :: old.GL_COLOR_BUFFER_BIT;
    DEPTH_BIT :: old.GL_DEPTH_BUFFER_BIT;
    STENCIL_BIT :: old.GL_STENCIL_BUFFER_BIT;
}

clear :: inline (mask: FramebufferMask){
    old.glClear(xx mask);
}

DrawMode :: enum {
    POINTS :: old.GL_POINTS;
    
    LINES :: old.GL_LINES;
    LINE_LOOP :: old.GL_LINE_LOOP;
    LINE_STRIP :: old.GL_LINE_STRIP;
    LINE_STRIP_ADJACENCY :: old.GL_LINE_STRIP_ADJACENCY;
    LINES_ADJACENCY :: old.GL_LINES_ADJACENCY;
    
    TRIANGLES :: old.GL_TRIANGLES;
    TRIANGLE_STIP :: old.GL_TRIANGLE_STRIP;
    TRIANGLE_FAN :: old.GL_TRIANGLE_FAN;
    TRIANGLE_STRIP_ADJACENCY :: old.GL_TRIANGLE_STRIP_ADJACENCY;
    TRIANGLES_ADJACENCY :: old.GL_TRIANGLES_ADJACENCY;
    PATCHES :: old.GL_PATCHES;
}

DrawType :: enum {
    U8 :: old.GL_UNSIGNED_BYTE;
    U16 :: old.GL_UNSIGNED_SHORT;
    U32 :: old.GL_UNSIGNED_INT;
}

drawElements :: inline (mode: DrawMode, count: u32, type: DrawType, indices: *void = null){
    old.glDrawElements(xx mode, count, xx type, indices);
}

drawArrays :: inline (mode: DrawMode, count: s64, first: s64 = 0){
    old.glDrawArrays(xx mode, xx first,  xx count);
}

viewport :: (x: s64, y: s64, width: s64, height: s64){
    old.glViewport(xx x, xx y, xx width, xx height);
}

//### Textures ##//
Texture :: #type,distinct u32;

TextureTargets :: enum{
    TEXTURE_1D :: old.GL_TEXTURE_1D; 
    TEXTURE_2D :: old.GL_TEXTURE_2D; 
    TEXTURE_3D :: old.GL_TEXTURE_3D;
    //@Incomplete
    //GL_TEXTURE_1D_ARRAY, 
    //GL_TEXTURE_2D_ARRAY, 
    //GL_TEXTURE_RECTANGLE,
    TEXTURE_CUBE_MAP :: old.GL_TEXTURE_CUBE_MAP;
    //GL_TEXTURE_CUBE_MAP_ARRAY, 
    TEXTURE_BUFFER :: old.GL_TEXTURE_BUFFER;
    TEXTURE_2D_MULTISAMPLE :: old.GL_TEXTURE_2D_MULTISAMPLE;
    //GL_TEXTURE_2D_MULTISAMPLE_ARRAY.
}

createTexture :: (target: TextureTargets) -> Texture{
    texture: Texture;
    old.glCreateTextures(xx target, 1, xx *texture);
    return texture;
}

deleteTexture :: (textures: Texture){
    old.glDeleteTextures(1, xx *textures);
}

SizedInternalFormats :: enum{
    RGBA8 :: old.GL_RGBA8;
    //@@@ ULTRA_Incomplete
}

textureStorage2DMultisample :: (texture: Texture, samples: s64, internalformat: SizedInternalFormats, width: s64, height: s64, fixedsamplelocations: bool){
    old.glTextureStorage2DMultisample(xx texture, xx samples, xx internalformat, xx width, xx height, xx fixedsamplelocations);
}

//### Framebuffers ##//
Framebuffer :: #type,distinct u32;

createFramebuffer :: inline () -> Framebuffer{
    fbuff: Framebuffer;
    old.glCreateFramebuffers(1, xx *fbuff);
    return fbuff;
}

deleteFramebuffer :: (framebuffer: Framebuffer){
    old.glDeleteFramebuffers(1, xx *framebuffer);
}

FramebufferAttachment :: enum{
    COLOR_ATTACHMENT0 :: old.GL_COLOR_ATTACHMENT0; //@Incomplete (other attachment numbers)
    DEPTH_ATTACHMENT :: old.GL_DEPTH_ATTACHMENT;
    STENCIL_ATTACHMENT :: old.GL_STENCIL_ATTACHMENT;
    DEPTH_STENCIL_ATTACHMENT :: old.GL_DEPTH_STENCIL_ATTACHMENT;
}

framebufferTexture :: (framebuffer: Framebuffer, attachment: FramebufferAttachment, texture: Texture, level: s64){
    old.glNamedFramebufferTexture(xx framebuffer, xx attachment, xx texture, xx level);
}

BindFramebufferTarget :: enum{
    DRAW_FRAMEBUFFER :: old.GL_DRAW_FRAMEBUFFER; 
    READ_FRAMEBUFFER :: old.GL_READ_FRAMEBUFFER;
    FRAMEBUFFER :: old.GL_FRAMEBUFFER;
}

bindFramebuffer :: (target: BindFramebufferTarget, framebuffer: Framebuffer){
    old.glBindFramebuffer(xx target, xx framebuffer);
}

BlitFilter :: enum {
    NEAREST :: old.GL_NEAREST; 
    LINEAR :: old.GL_LINEAR;
}

blitFramebuffer :: (readFramebuffer: Framebuffer, drawFramebuffer: Framebuffer, 
                    srcX0: s64, srcY0: s64, srcX1: s64, srcY1: s64, 
                    dstX0: s64, dstY0: s64, dstX1: s64, dstY1: s64, 
                    mask: FramebufferMask, filter: BlitFilter){
    old.glBlitNamedFramebuffer(xx readFramebuffer, xx drawFramebuffer, 
                               xx srcX0, xx srcY0, xx srcX1, xx srcY1, 
                               xx dstX0, xx dstY0, xx dstX1, xx dstY1, 
                               xx mask, xx filter);
}

/// @HACK
swap_buffers :: (hwnd: HWND) {
    dc := GetDC(hwnd);
    result := SwapBuffers(dc);
}

