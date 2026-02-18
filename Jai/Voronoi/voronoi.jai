#import "Basic";
#import "Window_Creation";
#import "Input";
#import "Debug";
#import "File";
#import "Math";
#import "Helpers";
#import "Windows";
#import "Windows_Utf8";
gl :: #import,dir "../CLGL";
#import "Random";

win_width := 1280;
win_height := 900;
SAMPLES :: 16;
APP_NAME :: "pepe";

createShader :: (vertFilePath: string, fragFilePath: string) -> gl.Program{
    
    compileShader :: (type: gl.ShaderType, source: string) -> gl.Shader{

        id := gl.createShader(type);
        gl.shaderSource(id, source);
        gl.compileShader(id);

        result := gl.getShaderCompileStatus(id);

        if(result == false){
            message := gl.getShaderInfoLog(id);
            print("ERROR: %\n", message);
            free(message.data);

            gl.deleteShader(id);

            return xx 0;
        }

        return id;
    }

    vertSource := read_entire_file(vertFilePath);
    defer free(vertSource);
    fragSource := read_entire_file(fragFilePath);
    defer free(fragSource);

    program := gl.createProgram();
    vShader := compileShader(.VERTEX, vertSource);
    fShader := compileShader(.FRAGMENT, fragSource);

    gl.attachShader(program, vShader);
    gl.attachShader(program, fShader);
    gl.linkProgram(program);
    gl.validateProgram(program);

    gl.deleteShader(vShader);
    gl.deleteShader(fShader);

    return program;
}

generate_circle :: (n: int, r: f32, border_thickness: f32) -> [][3]f32, [][3]f32{
    circle_points: [..][3]float;
    border_points: [..][3]float;

    cx, cy := 0.0, 0.0;

    for i: 0..n-1{
        theta_1 := i*TAU/n;
        theta_2 := (i+1)*TAU/n;

        // Circle
        {
            x1, y1 := cx + r*cos(theta_1), cy + r*sin(theta_1);
            x2, y2 := cx + r*cos(theta_2), cy + r*sin(theta_2);

            array_add(*circle_points, f32.[cx, cy, 0], f32.[x1, y1, 0], f32.[x2, y2, 0]);
        }

        // Border
        {
            r_low := r-border_thickness/2;
            r_high := r+border_thickness/2;
            
            x1, y1 := cx + r_high*cos(theta_2), cy + r_high*sin(theta_2);
            x2, y2 := cx + r_high*cos(theta_1), cy + r_high*sin(theta_1);
            x3, y3 := cx + r_low*cos(theta_1), cy + r_low*sin(theta_1);
            x4, y4 := cx + r_low*cos(theta_2), cy + r_low*sin(theta_2);

            array_add(*border_points, f32.[x1, y1, 0], f32.[x2, y2, 0], f32.[x3, y3, 0]);
            array_add(*border_points, f32.[x1, y1, 0], f32.[x4, y4, 0], f32.[x3, y3, 0]);
        }

    }

    return circle_points, border_points;
}

Voronoi_Point :: struct {
    position: [4]f32 = .[0,0,0,1];
    radius: f32;
    color: [4]f32;
}

voronoi_points: struct {
    initialized := false;
    
    circle_vertBuffID: gl.Buffer;
    circle_vertArrayID: gl.VertexArray;

    border_vertBuffID: gl.Buffer;
    border_vertArrayID: gl.VertexArray;

    shaderID: gl.Program;
    circle_vertex_count: int;
    border_vertex_count: int;

    ucolor_loc: gl.UniformLocation;
    VP_Mat_loc: gl.UniformLocation;
    obj_M_Mat_loc: gl.UniformLocation;

    array: [..]*Voronoi_Point;
    point_dragging: *Voronoi_Point;
};

init_voronoi_points :: (){
    using voronoi_points;
    initialized = true;
    
    // Generate Meshes
    circle_mesh, border_mesh := generate_circle(100, 1, 0.1);
    defer array_free(circle_mesh);
    defer array_free(border_mesh);
    circle_vertex_count = circle_mesh.count;
    border_vertex_count = border_mesh.count;

    // Circle Mesh Vertex Buffer
    circle_vertBuffID = gl.createBuffer();
    gl.bufferStorage(circle_vertBuffID, circle_mesh);

    // Circle Mesh Vertex Array
    circle_vertArrayID = gl.createVertexArray();
    gl.vertexArrayVertexBuffer(circle_vertArrayID, 0, circle_vertBuffID, 0, 3*size_of(f32));

    gl.enableVertexArrayAttrib(circle_vertArrayID, 0);
    gl.vertexArrayAttribFormat(circle_vertArrayID, 0, 3, .F32, 0);
    gl.vertexArrayAttribBinding(circle_vertArrayID, 0, 0);

    // Border Mesh Vertex Buffer
    border_vertBuffID = gl.createBuffer();
    gl.bufferStorage(border_vertBuffID, border_mesh);

    // Border Mesh Vertex Array
    border_vertArrayID = gl.createVertexArray();
    gl.vertexArrayVertexBuffer(border_vertArrayID, 0, border_vertBuffID, 0, 3*size_of(f32));

    gl.enableVertexArrayAttrib(border_vertArrayID, 0);
    gl.vertexArrayAttribFormat(border_vertArrayID, 0, 3, .F32, 0);
    gl.vertexArrayAttribBinding(border_vertArrayID, 0, 0);

    // Mesh Shader
    shaderID = createShader("Shaders/solid_color_vert.glsl", "Shaders/solid_color_frag.glsl");
    ucolor_loc = gl.getUniformLocation(shaderID, "ucolor");
    VP_Mat_loc = gl.getUniformLocation(shaderID, "VP_Mat");
    obj_M_Mat_loc = gl.getUniformLocation(shaderID, "M_Mat");
}

create_voronoi_point :: (iposition: [2]f32, radius: f32, color: [3]f32){
    if !voronoi_points.initialized then init_voronoi_points();
    
    point := new(Voronoi_Point);

    point.position[0] = iposition[0];
    point.position[1] = iposition[1];

    point.radius = radius;
    
    point.color[0] = color[0];
    point.color[1] = color[1];
    point.color[2] = color[2];

    array_add(*voronoi_points.array, point);
}

draw_voronoi_points :: (VP_Mat: Matrix4){
    using voronoi_points;

    for <point: array{
        using point;
        gl.bindProgram(shaderID);
        obj_M_Mat := Matrix4_Identity;
        obj_M_Mat._14 = position[0];
        obj_M_Mat._24 = position[1];
        obj_M_Mat._11 = radius;
        obj_M_Mat._22 = radius;
        gl.uniformMatrix4fv(VP_Mat_loc, true, VP_Mat.floats.data);
        gl.uniformMatrix4fv(obj_M_Mat_loc, true, obj_M_Mat.floats.data);

        // Circle
        gl.bindVertexArray(circle_vertArrayID);
        gl.uniform4fv(ucolor_loc, color.data);
        gl.drawArrays(.TRIANGLES, circle_vertex_count);

        // Border
        gl.bindVertexArray(border_vertArrayID);
        color_black := f32.[0,0,0,1];
        
        gl.uniform4fv(ucolor_loc, color_black.data);
        gl.drawArrays(.TRIANGLES, border_vertex_count);
    }
    
}

update_voronoi_points :: (mx: f32, my: f32){

    if voronoi_points.point_dragging != null{
        voronoi_points.point_dragging.position[0] = mx;
        voronoi_points.point_dragging.position[1] = my;
    } 

    hovered_point: *Voronoi_Point = null;
    for point: voronoi_points.array{
        
        dx := mx-point.position[0];
        dy := my-point.position[1];
        if dx*dx + dy*dy <= point.radius*point.radius{
            hovered_point = point;
            break;
        }
    }
    
    for event: events_this_frame {
        if event.type == {
            case .KEYBOARD;
                if event.key_code == .MOUSE_BUTTON_LEFT{
                    if event.key_pressed{
                        voronoi_points.point_dragging = hovered_point;
                    }else{
                        voronoi_points.point_dragging = null;
                    }
                }
        }
    }

}

main :: (){
    
    //### Init Window and OpenGL ###//
    main_window := create_window(win_width, win_height, APP_NAME);
    gl.old.gl_create_context(main_window, 4, 5, compatibility = true, debug = true, wanted_msaa = 16);
    gl.old.gl_load(*gl.old.gl);
    gl.enable_debug_output();
    gl.old.wglSwapIntervalEXT(1);

    major_v: s32;
    gl.old.glGetIntegerv(gl.old.GL_MAJOR_VERSION, *major_v);
    minor_v: s32;
    gl.old.glGetIntegerv(gl.old.GL_MINOR_VERSION, *minor_v);
    print("OPENGL VERSION: %.%\n", major_v, minor_v);

    //### Generate Points ###//
    
    /*create_voronoi_point(.[0,0], 0.05, .[1, 0, 0]);
    create_voronoi_point(.[1,0], 0.05, .[0, 1, 0]);
    create_voronoi_point(.[0,1], 0.05, .[0, 0, 1]);
    create_voronoi_point(.[1,1], 0.05, .[0, 0.5, 1]);
    create_voronoi_point(.[-1,1], 0.05, .[0.5, 0, 1]);*/

    random_seed(7);
    for i: 0..30{
        x := random_get_within_range(-3, 3);
        y := random_get_within_range(-3, 3);

        r := random_get_within_range(0, 1);
        g := random_get_within_range(0, 1);
        b := random_get_within_range(0, 1);

        create_voronoi_point(.[x,y], 0.05, .[r, g, b]);
    }
    
    //### Voronoi Background ###//

    // Background Vertex Buffer
    background_mesh := f32.[
        -1, 1,   1, 1,   1, -1,
        -1, 1,   1, -1,  -1, -1,
    ];

    background_vertBuffID := gl.createBuffer();
    gl.bufferStorage(background_vertBuffID, background_mesh);

    // Background Vertex Array
    background_vertArrayID := gl.createVertexArray();
    gl.vertexArrayVertexBuffer(background_vertArrayID, 0, background_vertBuffID, 0, 2*size_of(f32));

    gl.enableVertexArrayAttrib(background_vertArrayID, 0);
    gl.vertexArrayAttribFormat(background_vertArrayID, 0, 2, .F32, 0);
    gl.vertexArrayAttribBinding(background_vertArrayID, 0, 0);

    // Point Buffer
    point_buffer := gl.createBuffer();
    gl.bufferStorage(point_buffer, 4*size_of(f32)*voronoi_points.array.count, null, .DYNAMIC_STORAGE);
    gl.bindBufferBase(.SHADER_STORAGE_BUFFER, 0, point_buffer);

    // Point Color Buffer
    point_color_buffer := gl.createBuffer();
    gl.bufferStorage(point_color_buffer, 4*size_of(f32)*voronoi_points.array.count, null, .DYNAMIC_STORAGE);
    gl.bindBufferBase(.SHADER_STORAGE_BUFFER, 1, point_color_buffer);

    // Background Shader
    background_shaderID := createShader("Shaders/background_vert.glsl", "Shaders/background_frag.glsl");
    background_win_width_loc := gl.getUniformLocation(background_shaderID, "win_width");
    background_win_height_loc := gl.getUniformLocation(background_shaderID, "win_height");
    background_inv_VP_Mat_loc := gl.getUniformLocation(background_shaderID, "inv_VP_Mat");
    background_listSize_loc := gl.getUniformLocation(background_shaderID, "listSize");

    //## Multisamples Framebuffer ##//
    fbo := gl.createFramebuffer();
    texture := gl.createTexture(.TEXTURE_2D_MULTISAMPLE);
    gl.textureStorage2DMultisample(texture, SAMPLES, .RGBA8, win_width, win_height, true);
    gl.framebufferTexture(fbo, .COLOR_ATTACHMENT0, texture, 0);

    //### MVP Matrices ###//
    cam_pos_x, cam_pos_y, cam_zoom: f32 = 0.3, 0.3, 0.5;
    
    aspect_ratio := win_width/cast(float)(win_height);
    P_Mat := Matrix4_Identity;
    V_Mat := Matrix4_Identity;
    VP_Mat := P_Mat*V_Mat;

    //### Main Loop ###//
    dt: f32;
    ti, tf := get_time(), get_time();
    fps_timer: f32 = 0;

    dragging := false;
    slow_zoom_step :: 0.1;
    fast_zoom_step :: 0.3;
    zoom_step := slow_zoom_step;

    m_prev_pos, m_pos: Vector4;

    quit := false;
    while(!quit){
        
        // Input
        update_window_events();

        if input_button_states[xx Key_Code.SHIFT]{
            zoom_step = fast_zoom_step;
        }else{
            zoom_step = slow_zoom_step;
        }
        
        for event: events_this_frame {
            if event.type == {
                case .QUIT;
                    quit = true;
            }

            if event.type == {
                case .KEYBOARD;
                    if event.key_code == .MOUSE_BUTTON_MIDDLE{
                        if event.key_pressed{
                            dragging = true;
                        }else{
                            dragging = false;
                        }
                    }
                    else if event.key_code == .F4 && event.alt_pressed{
                        quit = true;
                    }
                
                case .MOUSE_WHEEL;
                    cam_zoom = max(cam_zoom + cam_zoom/2*zoom_step*event.wheel_delta*dt, 0.01); // @@WTF, make this zoom framerate independant...
            }
        }

        resizes := get_window_resizes();
        for record: resizes {
            if record.window == main_window {
                win_width = record.width;
                win_height = record.height;
                aspect_ratio = win_width/cast(float)(win_height);

                gl.deleteFramebuffer(fbo);
                gl.deleteTexture(texture);

                fbo = gl.createFramebuffer();
                texture = gl.createTexture(.TEXTURE_2D_MULTISAMPLE);
                gl.textureStorage2DMultisample(texture, SAMPLES, .RGBA8, win_width, win_height, true);
                gl.framebufferTexture(fbo, .COLOR_ATTACHMENT0, texture, 0);

                gl.viewport(0, 0, win_width, win_height);
            }
        }
        
        mx_wind, my_wind := get_mouse_pointer_position(main_window, true);
        mx_norm, my_norm := clamp(mx_wind/cast(f32)(win_width)*2- 1, -1, 1), clamp(my_wind/cast(f32)(win_height)*2 - 1, -1, 1);
        inv_VP_Mat := inverse(VP_Mat);
        m_pos = inv_VP_Mat*Vector4.{mx_norm, my_norm, 0, 1}; // @TODO: hay un delay extraño cuando el frame rate está limitado por la velocidad del programa y no por el vsync

        if dragging{
            cam_pos_x -= (m_pos.x-m_prev_pos.x);
            cam_pos_y -= (m_pos.y-m_prev_pos.y);
        }
        
        P_Mat = Matrix4_Identity;
        P_Mat._11 = cam_zoom;
        P_Mat._22 = cam_zoom*aspect_ratio;

        V_Mat = Matrix4_Identity;

        V_Mat._14 = -cam_pos_x;
        V_Mat._24 = -cam_pos_y;

        VP_Mat = P_Mat*V_Mat;

        update_voronoi_points(m_pos.x, m_pos.y);

        // Render
        gl.bindFramebuffer(.FRAMEBUFFER, fbo);
        gl.clear(.COLOR_BIT);
        
        gl.bindVertexArray(background_vertArrayID);
        gl.bindProgram(background_shaderID);

        gl.uniformMatrix4fv(background_inv_VP_Mat_loc, true, inv_VP_Mat.floats.data);
        gl.uniform1i(background_win_width_loc, xx win_width);
        gl.uniform1i(background_win_height_loc, xx win_height);
        gl.uniform1i(background_listSize_loc, xx voronoi_points.array.count);

        for point, i: voronoi_points.array{
            gl.bufferSubData(point_buffer, 4*i*size_of(f32), 4*size_of(f32), point.position.data);
            gl.bufferSubData(point_color_buffer, 4*i*size_of(f32), 4*size_of(f32), point.color.data);
        }
        
        gl.drawArrays(.TRIANGLES, background_mesh.count/2);

        // v
        draw_voronoi_points(VP_Mat);
        
        // End Frame
        gl.bindFramebuffer(.FRAMEBUFFER, xx 0);
        gl.clear(.COLOR_BIT);
        gl.blitFramebuffer(fbo, 0,
                           0, 0, win_width, win_height,
                           0, 0, win_width, win_height,     
                           .COLOR_BIT, .NEAREST);

        m_pos = inverse(VP_Mat)*Vector4.{mx_norm, my_norm, 0, 1};
        m_prev_pos = m_pos;

        gl.swap_buffers(main_window);
        tf = get_time();
        dt = cast(f32)(tf-ti);
        fps_timer += dt;
        if fps_timer >= 0.1{    
            frame_indicator := sprint("% - % FPS, %ms", APP_NAME, cast(int)(1/dt), dt*1000);
            SetWindowTextW(main_window, utf8_to_wide(frame_indicator));
            fps_timer = 0;
            free(frame_indicator);
        }
        ti = tf;
        
    }
}


