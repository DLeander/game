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
    // Init player shader
    m_playerShader = new CSHADER("resources/shaders/default.vert", "resources/shaders/default.frag");
    
    m_player = new CPLAYER(m_playerShader);

    m_camera = new CCAMERA(m_iWindowWidth, m_iWindowHeight, glm::vec3(0.0f, 0.0f, 2.0f));

    // Init terrain
    m_terrain.faultFormation(0, 255, 16, 128, "resources/terrain/heightmap.png");
    m_terrain.init("resources/terrain/heightmap.raw", 128, 0.2f);

    // Do wireframe mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

// Start the client game loop.
void CCLIENT::loop(){
    // Main game loop
	while (!glfwWindowShouldClose(m_window))
	{
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

}

// Mouse button callback function for the client.
void CCLIENT::mouse_button_callback(GLFWwindow* window, int button, int action, int mode){

}

// Clean up the allocated files.
void CCLIENT::cleanup(){
    // ORDER OF DELETION IS IMPORTANT:

    // Delete player object
    delete m_player;
    // Delete window before ending the program
	glfwDestroyWindow(m_window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}