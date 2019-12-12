//
//  main.cpp
//  SoftDrop
//
//  Created by James Hovet on 7/14/19.
//  Copyright © 2019 James Hovet. All rights reserved.
//

#define private public // EVIL HACK

#include "main.h"


int handleArguments(int argc, const char * argv[]);
int runNestest();
int run_instr_test_v5();
int livePlay(std::string fileName, int spritesheet);

static void hexDump(Core &cpu, unsigned short start, unsigned short end);
static void hexDumpCPU(Mapper& m, unsigned short start, unsigned short end);
static void hexDumpPPU(Mapper& m, unsigned short start, unsigned short end);

bool window_init();
void window_close();
int runPPUTestOne();
void holdWindowOpen();

const int SCREEN_WIDTH = 256 * 3;
const int SCREEN_HEIGHT = 240 * 3;

SDL_Window* g_window = NULL;
SDL_Renderer* g_renderer = NULL;
SDL_GLContext gl_context;

DebugOptions* debugOptions = nullptr;

int main(int argc, const char * argv[]) {
    int statusCode = 0;
    debugOptions = new DebugOptions();
    
    //arguments code path
    if(argc > 0){
        statusCode = handleArguments(argc, argv);
    } else {
    //"default", argumentless code path
        Log::g_filter = Log::Level::Main;
    
//    Mapper000 map;
//    std::ifstream file ("../../Roms/nestest.nes", std::ios::in|std::ios::binary|std::ios::ate);
//    if(!file.is_open()){
//        std::cout << "Unable to open file";
//        return 1;
//    }
//    map.readINES(file);
//    file.close();
//
        window_init();
//
//    PPU ppu = PPU(map, g_renderer);
//
//    map.setByte(0x2006, '\x20');
//    map.setByte(0x2006, '\x00');
//    for(int i = 0; i < 0x400; i++){
//        map.setByte(0x2007, ((i % 32) + (i / 32)) % 32 + 0x41);
//    }
//    SDL_RenderClear(g_renderer);
////    hexDumpPPU(map, 0x2000, 0x2400);
////    ppu.renderNametable(0x2000, 0);
//    ppu.renderSpritesheet(0);
//    SDL_RenderPresent(g_renderer);
//
        holdWindowOpen();
        window_close();

//    unsigned int g_filter = Log::Level::Controller;
    
//    auto ret = livePlay("../../Roms/nestest.nes", 0);
//    auto ret = livePlay("../../Roms/Tetris.nes", 7);

//    auto ret = runPPUTestOne();
//    auto ret = runNestest();
//    auto ret = run_instr_test_v5();

//    statusCode = ret;
    }
    
    //clean up
    delete debugOptions;
    
    return statusCode;
}

static void handleLogCommandLineArg(const boost::program_options::variables_map &vm) {
    std::vector<std::string> inputLogLevelStrings = vm["log"].as<std::vector<std::string>>();
    for(std::string s : inputLogLevelStrings){
        std::cout << s << "\n";
        Log::logLevelString v0 = Log::logLevelStringMap.at(s);
        std::cout << v0.name << " " << v0.level << "\n";
        Log::g_filter |= v0.level;
    }
}

int handleArguments(int argc, const char * argv[]){
    namespace po = boost::program_options;
    
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "produce help message")
    ("log", po::value<std::vector<std::string>>(), "output log messages with a given log level")
    ("nestest_automatic", "run nestest automatic")
    ("play", po::value<std::string>(), "live play a given .nes rom")
    ("imgui", "enable the imgui overlay")
    ;
    
    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc, argv).options(desc).allow_unregistered().run();
    po::store(parsed, vm);
    po::notify(vm);
    
    if (vm.count("log")) {
        handleLogCommandLineArg(vm);
    }
    
    if(vm.count("imgui")){
        debugOptions->shouldDrawImGui = true;
    }
    
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    if (vm.count("nestest_automatic")){
        std::cout << "nestest automatic not yet implimented";
        return 1;
    }

    if (vm.count("play")){
        std::string fileName = vm["play"].as<std::string>();
        return livePlay(fileName, 0);
    }

    return 0;
}

bool window_init()
{
    //Initialization flag
    bool success = true;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        const char* glsl_version = "#version 110";
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        
        //Create window
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        g_window = SDL_CreateWindow("SoftDrop", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        gl_context = SDL_GL_CreateContext(g_window);
        SDL_GL_MakeCurrent(g_window, gl_context);
        SDL_GL_SetSwapInterval(1); // Enable vsync
        
        glewExperimental = GL_TRUE;
        bool glewErr = glewInit() != GLEW_OK;
        
        if( g_window == NULL || g_renderer == NULL)
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
//        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        ImGui::StyleColorsDark();
        
        ImGui_ImplSDL2_InitForOpenGL(g_window, gl_context);
        ImGui_ImplOpenGL3_Init(glsl_version);
        
        
    }
    SDL_RenderClear(g_renderer);
    return success;
}


void window_close()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    
    SDL_GL_DeleteContext(gl_context);
    
    SDL_DestroyRenderer(g_renderer);
    g_renderer = NULL;

    //Destroy window
    SDL_DestroyWindow( g_window );
    g_window = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void holdWindowOpen() {
    SDL_Event e;
    bool quit = false;
    while (!quit){
        //        ppu.renderNametable(ppuDump + 0x2000, 3);
        //        ppu.renderSprites(3);
        //        SDL_RenderPresent(gRenderer);
        while (SDL_PollEvent(&e)){
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_KEYDOWN){
                quit = false;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = false;
            }
        }
    }
}

//---------------------------------------------------------------------

static void drawLoggingFilterButtons(){
    ImGui::Checkbox("log to stdout", &(debugOptions->shouldLogToStdout));
    for(auto s : Log::logLevelStrings){
        Log::Level level = Log::logLevelStringMap.at(s.name).level;
        bool doesCurrentLevelPassFilter = Log::doesPassFilter(level);
        ImGui::SameLine();
        if(doesCurrentLevelPassFilter){
            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1.0f, 0.8f, 0.8f));
        }
        if(ImGui::Button(s.name)){
            if(!doesCurrentLevelPassFilter){
                Log::g_filter |= level;
            } else {
                Log::g_filter &= UINT_MAX - level;
            }
        }
        if(doesCurrentLevelPassFilter){
            ImGui::PopStyleColor();
        }
    }
}




static void drawImGuiGeneralOptions(Mapper& map, PPU& ppu, unsigned long& frame) {
    ImGui::Begin("General Options");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
    if(ImGui::Button("Pause/Play")){
        debugOptions->isPaused = !debugOptions->isPaused;
    }
    ImGui::SameLine();
    if(ImGui::Button("Advance Frame")){
        debugOptions->shouldAdvanceOneFrame = true;
    }
    ImGui::SameLine(); ImGui::Text("frame: %lu", frame);
    ImGui::SameLine();
    if(ImGui::Button("Reset")){
        debugOptions->shouldReset = true;
    }
    
    ImGui::Checkbox("override spritesheet?", &(debugOptions->shouldOverrideSpritetable));
    if(debugOptions->shouldOverrideSpritetable){
        ImGui::SameLine(); ImGui::InputInt("spritesheet", &(debugOptions->spritesheetOverride));
    }
    
    ImGui::Checkbox("draw sprites", &(debugOptions->shouldDrawSprites));
    ImGui::Checkbox("draw nametables", &(debugOptions->shouldDrawNametables));
    
    drawLoggingFilterButtons();
    
    ImGui::Checkbox("log Ops", &(debugOptions->shouldLogOps));
    
//    unsigned short addrForDump = 0;
    static char addrCStringToConvert [5] = {};
    static char endOfDumpCStringToConvert [5] = {};
    ImGui::InputText("Address To Start Dump", addrCStringToConvert, 5);
    ImGui::InputText("Length of Dump", endOfDumpCStringToConvert, 5);
    if(ImGui::Button("Hex Dump CPU")){
        unsigned short start = Utils::convertHexStringToUnsignedShort(addrCStringToConvert, 4);
        unsigned short end = start +
            Utils::convertHexStringToUnsignedShort(endOfDumpCStringToConvert, 4);
        hexDumpCPU(map, start, end);
    }
    ImGui::SameLine();
    if(ImGui::Button("Hex Dump PPU")){
        unsigned short start = Utils::convertHexStringToUnsignedShort(addrCStringToConvert, 4);
        unsigned short end = start +
            Utils::convertHexStringToUnsignedShort(endOfDumpCStringToConvert, 4);
        hexDumpPPU(map, start, end);
    }
    
    ImGui::End();
}

//---------------------------------------------------------------------

int livePlay(std::string gameName, int spritesheet) {
    Mapper001 map;
    std::ifstream file (gameName, std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();

    window_init();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    Core cpu = Core(map);
    int status = 0;
    PPU ppu = PPU(map, g_renderer);

    cpu.reset();

    int STEPS_PER_FRAME = 29781;
    int VBLANK_START = 27393;

    unsigned long frame = 0;

    const int SCREEN_FPS = 60;
    const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

    Uint32 startTime = SDL_GetTicks();

    SDL_Event e;
    bool quit = false;
    while (!quit){
        //poll for input
        startTime = SDL_GetTicks();
        while (SDL_PollEvent(&e)){
            ImGui_ImplSDL2_ProcessEvent(&e);
            if (e.type == SDL_QUIT){
                quit = true;
            }
            if (e.type == SDL_MOUSEBUTTONDOWN){
                quit = false;
            }
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_i){
                debugOptions->shouldDrawImGui = !debugOptions->shouldDrawImGui;
                quit = false;
            }
            if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_p){
                debugOptions->isPaused = !debugOptions->isPaused;
            }
        }
        
        //------------Imgui New Frame----------------
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame(g_window);
        ImGui::NewFrame();
        
        bool show_demo_window = true;
        ImGui::ShowDemoWindow(&show_demo_window);
        if(debugOptions->shouldDrawImGui){
            drawImGuiGeneralOptions(map, ppu, frame);
            
        }
        
        //------------Populate Controller Input----------------
        map.setButtonValue(0);
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        for(controllerButton pair : keyMappings){
            if(state[pair.key]){
                map.orButtonValue(pair.button);
                logf(Log::Level::Controller, "Key Pressed: %s\n", pair.buttonName.c_str());
            }
        }
        
        //------------Reset if need be-------------
        if(debugOptions->shouldReset){
            cpu.reset();
            frame = 0;
            debugOptions->shouldReset = false;
        }
        
        
        //------------Run One Frame----------------
        if(!debugOptions->isPaused || debugOptions->shouldAdvanceOneFrame){

            if(debugOptions->shouldPrintFrameNumbers){
                std::cout << "--------Frame " << frame << "--------" << std::endl;
            }
            
            map.clearVBlank();
            status = cpu.stepTo(frame * STEPS_PER_FRAME + VBLANK_START);
            if(status != 0){
                printf("[Main][Error]BRK\n");
            }
            cpu.setNMI();
            map.setVBlank();
            cpu.stepTo(frame * STEPS_PER_FRAME + STEPS_PER_FRAME);

            frame++;
            debugOptions->shouldAdvanceOneFrame = false;
        }
        
        
        //------------Draw Call----------------
        
        //TODO: spritesheet stuff to do for real.
        SDL_RenderClear(g_renderer);
        if(debugOptions->shouldDrawNametables)
            ppu.renderNametable(map.getPPUPointerAt(0x2000), debugOptions->spritesheetOverride);
//        hexDumpPPU(map, 0x2000, 0x2400);
//        ppu.renderNametable(0x2000, 7);
//        ppu.renderNametable(map.VRAM, 0);
        if(debugOptions->shouldDrawSprites)
            ppu.renderSprites(debugOptions->spritesheetOverride); // tetris 7
//        ppu.renderSpritesheet(0);
        
        //imgui drawing
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        SDL_RenderPresent(g_renderer);

        //------------Post Draw----------------
        Uint32 currentTime = SDL_GetTicks();
        Uint32 frameTicks = currentTime - startTime;
        if(frameTicks < SCREEN_TICKS_PER_FRAME){
            Uint32 delay = SCREEN_TICKS_PER_FRAME - frameTicks;
            logf(Log::Level::Main, "Delaying %u\n", delay);
            SDL_Delay(delay);
        }

    }

    window_close();
    return 0;
}
//---------------------------------------------------------------------
int runPPUTestOne() {
    Mapper001 map;
    std::ifstream file ("Tetris.nes", std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();

    std::ifstream ppuDumpFile ("ppuGreen.b", std::ios::in|std::ios::binary|std::ios::ate);
    if(!ppuDumpFile.is_open()){
        std::cout << "unable to open ppu dump";
        return 1;
    }
    char ppuDump[0x4000];
    ppuDumpFile.seekg(0, std::ios::beg);
    ppuDumpFile.read(ppuDump, 0x4000);
    ppuDumpFile.close();

    memcpy(map.getPPUPointerAt(0x2000), ppuDump + 0x2000, 0x2000);

    std::ifstream oamDumpFile ("tetrisOAM.b", std::ios::in|std::ios::binary|std::ios::ate);
    if(!oamDumpFile.is_open()){
        std::cout << "unable to open OAM dump";
        return 1;
    }
    oamDumpFile.seekg(0, std::ios::beg);
    oamDumpFile.read(map.OAM, 0x100);
    oamDumpFile.close();

    window_init();


    PPU ppu = PPU(map, g_renderer);
    //    ppu.renderSpritesheet(3);
    //    ppu.renderSpritesheet(map.getPPUPointerAt(0) + 0x1000 * 0);
        ppu.renderNametable(ppuDump + 0x2000, 3);
//        ppu.renderAllColors();
        ppu.renderSprites(3);
        SDL_RenderPresent(g_renderer);


    holdWindowOpen();

    window_close();
    return 0;
}

static void hexDump(Core &cpu, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("[Main]\t");
        printf("$%04x: ", i);
        for(int j = 0; j < 0x10; j++){
            printf("%02x ", Utils::printable(cpu.m.getByte(i + j)));
        }
        for(int j = 0; j < 0x10; j++){
            char c = cpu.m.getByte(i + j);
            if(c > 31 && c < 127){
                std::cout << c;
            } else {
                std::cout << '.';
            }
        }
        printf("\n");
    }
}

static void hexDumpCPU(Mapper& m, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("[Main]\t");
        printf("$%04x: ", i);
        for(int j = 0; j < 0x10; j++){
            printf("%02x ", Utils::printable(m.getByte(i + j)));
        }
        for(int j = 0; j < 0x10; j++){
            char c = m.getByte(i + j);
            if(c > 31 && c < 127){
                std::cout << c;
            } else {
                std::cout << '.';
            }
        }
        printf("\n");
    }
}

static void hexDumpPPU(Mapper& m, unsigned short start, unsigned short end) {
    for(int i = start; i < end; i+=0x10){
        printf("[Main]\t");
        printf("$%04x: ", i);
        for(int j = 0; j < 0x10; j++){
            printf("%02x ", Utils::printable(m.getPPU(i + j)));
        }
        for(int j = 0; j < 0x10; j++){
            char c = m.getPPU(i + j);
            if(c > 31 && c < 127){
                std::cout << c;
            } else {
                std::cout << '.';
            }
        }
        printf("\n");
    }
}

int run_instr_test_v5() {
    //PC = ea71

    Mapper001 map;

    std::ifstream file ("official_only.nes", std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);
    file.close();


    Core cpu = Core(map);
    cpu.setPC(0xea71);


//    hexDump(cpu, 0xc000, 0xd000);

    long instructionCount = 0;
    while(cpu.step() == 0){
        if(instructionCount % 0x10000 == 0){
            std::cout << "------" << instructionCount << "------" << std::endl;
            hexDump(cpu, 0x6000, 0x6100);
            instructionCount += 0;
        }
        instructionCount++;
    }


    hexDump(cpu, 0x6000, 0x7000);

    return 0;
}

int runNestest(){
    Mapper000 map;

    std::ifstream file ("nestest.nes", std::ios::in|std::ios::binary|std::ios::ate);
    if(!file.is_open()){
        std::cout << "Unable to open file";
        return 1;
    }
    map.readINES(file);

    Core cpu = Core(map);
    cpu.setPC(0xc000); // automatic;
//    cpu.setPC(0xc004);

    return 0;
}
