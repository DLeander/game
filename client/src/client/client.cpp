#include "client/client.h"

// Init the client object.
CCLIENT::CCLIENT() : m_abSocketActive(true){
    init();
}

// Call cleanup to delete the object.
CCLIENT::~CCLIENT(){
    cleanup();
}

// Run the client by calling the game loop.
void CCLIENT::run(){
    // Create a thread for the package receive loop.
    m_vThreads.push_back(std::thread(&CCLIENT::receivePlayersInfo, this));
    // Create a thread for the package send loop.
    m_vThreads.push_back(std::thread(&CCLIENT::sendPlayerInfo, this));
    loop();

    for (std::thread &t : m_vThreads){
        t.join();
    }
}

void CCLIENT::init(){
    m_uuidClientID = m_network.connect();
    if (m_uuidClientID == boost::uuids::nil_uuid()){
        throw "Failed to connect to server after 25 tries.";
    }

    // Initialize GLFW
	glfwInit();
    // Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Create the window
    m_iWindowWidth = 800;
    m_iWindowHeight = 600;
    m_window = glfwCreateWindow(m_iWindowWidth, m_iWindowHeight, "Client Window", NULL, NULL);
    glfwMakeContextCurrent(m_window);
	// Error check if the window fails to create
	if (m_window == NULL)
	{
		glfwTerminate();
		throw "Failed to create GLFW window";
	}

    // Set the required callback functions
    glfwSetKeyCallback(m_window, key_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Load GLAD so it configures OpenGL
	int iVersion = gladLoadGL(glfwGetProcAddress);
    if (iVersion == 0)
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw "Failed to initialize OpenGL context";
    }
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, m_iWindowWidth, m_iWindowHeight);
    // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    // Init player shader
    m_playerShader = new CSHADER("resources/shaders/default.vert", "resources/shaders/default.frag");
    
    m_player = new CPLAYER(m_playerShader);

    m_camera = new CCAMERA(m_iWindowWidth, m_iWindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    // Init terrain
    // int iHeightMapHW = 128;
    // m_terrain.faultFormation(0, 255, 32, iHeightMapHW + 32, "resources/terrain/heightmap.png");
    // m_terrain.midPointDisplacement(128, "resources/terrain/heightmap.png");

    m_terrain.init("resources/terrain/heightmap_512.raw", 512, 0.5f);
    m_terrain.setupBuffers();

    // The different x and z directions we can have in slope lightning is
    // 1, 1 (45 degrees), 1, 0 (90 degrees), 1, -1 (135 degrees), 0, -1 (180 degrees), -1, -1 (225 degrees), -1, 0 (270 degrees), -1, 1 (315 degrees), 0, 1 (360 degrees)
    // The sun will always move in 45 degree increments, so the day/night cycle should only recalculate the lightning every "45 degrees" of suntime.
    m_terrain.setSlopeLightingParams(1, 1, 0.1f, 1.0f, 15.0f);
    m_terrain.calculateLightning();
}


// Start the client game loop.
void CCLIENT::loop(){

    // FPS counter
    double prevTime = 0.0;
    int crntTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;

    glfwSwapInterval(0);
    // Main game loop
	while (!glfwWindowShouldClose(m_window))
	{
        
        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;
        if (timeDiff >= 1.0/30.0){
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string ms = std::to_string((timeDiff/counter) * 1000);
            std::string title = "Client Window | FPS: " + FPS + " | ms: " + ms;
            glfwSetWindowTitle(m_window, title.c_str());
            prevTime = crntTime;
            counter = 0;
            // std::cout << "FPS: " << FPS << " | ms: " << ms << std::endl;
        }

        glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set up the client camera and read inputs
        // camera->inputs(window);

        // Render the world and players
        render();

        glfwSwapBuffers(m_window);
		// Take care of all GLFW events
		glfwPollEvents();

        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << err << std::endl;
        }
	}
    m_abSocketActive = false;
}

void CCLIENT::renderPlayers(){
    m_mtx.lock();
    std::unordered_map<boost::uuids::uuid, CPLAYER*> umLocalOtherPlayers = m_umOtherPlayers;
    m_mtx.unlock();
    for (auto& x : umLocalOtherPlayers){
        // If player is not initialized, initialize it.
        if (x.second->isInitialised() == false){
            x.second->init();
            x.second->setInitialised(true);
        }
        x.second->drawRemote();
    }
}

 void CCLIENT::renderWorld(){
    m_terrain.render(m_camera);
 }

void CCLIENT::sendPlayerInfo(){
    while (m_abSocketActive.load()){
        SPACKAGE package(PLAYER_INFO, m_uuidClientID, m_player->m_m4Model);
        m_network.sendToServer(package);
    }
}

void CCLIENT::receivePlayersInfo(){
    while (m_abSocketActive.load()){
        SPACKAGE package = m_network.receiveFromServer();
        if (package.s_uuidClientID == boost::uuids::nil_uuid()){
            continue;
        }
        m_mtx.lock();
        if (package.s_uuidClientID != m_uuidClientID && m_umOtherPlayers.find(package.s_uuidClientID) != m_umOtherPlayers.end()){
            // If the package is a disconnect package, remove the player from the map.
            if (package.s_ptType == PLAYER_DISCONNECT){
                std::cout << "Player disconnected" << std::endl;
                m_umOtherPlayers.erase(package.s_uuidClientID);
                m_mtx.unlock();
                continue;
            }
            m_umOtherPlayers[package.s_uuidClientID]->m_m4Model = package.s_m4Model;
            m_umOtherPlayers[package.s_uuidClientID]->setPositionFromModelMatrix();
        }
        else{
            // If the package is a disconnect package and the player is not previously loaded by the client, ignore the package.
            if (package.s_uuidClientID == m_uuidClientID || package.s_ptType == PLAYER_DISCONNECT){
                m_mtx.unlock();
                continue;
            }
            // If the player is not in the map, add the player to the map but do not initialize it, as it has to be done on the main thread (main loop()).
            m_umOtherPlayers[package.s_uuidClientID] = new CPLAYER(m_playerShader, package.s_m4Model);
            m_umOtherPlayers[package.s_uuidClientID]->setPositionFromModelMatrix();
        }
        m_mtx.unlock();
    }
}

// Render the client.
void CCLIENT::render() {
    // Render with wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    renderWorld();

    m_player->keyboard_input(m_window);
    m_player->mouse_input(m_window, m_camera);
    m_player->setupModelMatrix(m_camera);
    m_player->draw(m_camera);

    renderPlayers();
}

// Mouse callback functions for the client.
void CCLIENT::mouse_callback(GLFWwindow* window, double xpos, double ypos){

}

// Scroll callback function for the client.
void CCLIENT::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){

}

void CCLIENT::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    if (key == GLFW_KEY_G && action == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    if (key == GLFW_KEY_H && action == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

}

// Mouse button callback function for the client.
void CCLIENT::mouse_button_callback(GLFWwindow* window, int button, int action, int mode){

}

void CCLIENT::cleanup() {
    // Delete player object and check dependencies
    if (m_player) {
        delete m_player;
        m_player = nullptr;
    }

    // Delete player camera
    if (m_camera) {
        delete m_camera;
        m_camera = nullptr;
    }

    // Delete other players
    for (auto& x : m_umOtherPlayers) {
        if (x.second) {
            delete x.second;
            x.second = nullptr;
        }
    }

    // Ensure GLFW window is destroyed safely
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    // Terminate GLFW
    glfwTerminate();
}