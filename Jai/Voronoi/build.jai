
#import "Compiler";
#import "Process";
#import "Basic";

RUN :: true;
EXE_NAME :: "voronoi";
EXE_PATH :: "bin";

build :: () {

    // Create workspace
    w := compiler_create_workspace("Main Program");

    // Build options
    options := get_build_options(w);
    options.output_executable_name = EXE_NAME;
    options.output_path = EXE_PATH;
    options.output_type = .EXECUTABLE;
    //options.optimization_level = .DEBUG;
    options.backend = .X64;
    options.shorten_filenames_in_error_messages = true;
    options.text_output_flags = xx 0;

    set_build_options(options, w);

    // Intercept and build
    compiler_begin_intercept(w);
    add_build_file("voronoi.jai", w);

    error := false;

    while true {
        message := compiler_wait_for_message();

        if message.kind == {
            case .ERROR;
                error = true;
            case .COMPLETE;
                break;
        }
    }

    compiler_end_intercept(w);

    // Run
    if !error && RUN{
        print("\n--- RUNNING ---\n");
        run_command(sprint("%1/%2/%3.exe", #filepath, EXE_PATH, EXE_NAME));
    }

    // Set no output
    set_build_options_dc(.{do_output=false});
}

#run build();