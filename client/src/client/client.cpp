#include "client/client.h"

// Init the client object.
Client::Client() : socket_active(true){
    init();
}

// Call cleanup to delete the object.
Client::~Client(){
    cleanup();
}

// Run the client by calling the game loop.
void Client::run(){
    // Create a thread for the package receive loop.
    threads.push_back(std::thread(&Client::receivePlayersInfo, this));
    // Create a thread for the package send loop.
    threads.push_back(std::thread(&Client::sendPlayerInfo, this));
    loop();

    for (std::thread &t : threads){
        t.join();
    }
}

void Client::init(){
    clientID = network.connect();
    if (clientID == boost::uuids::nil_uuid()){
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
    window_width = 800;
    window_height = 600;
    window = glfwCreateWindow(window_width, window_height, "Client Window", NULL, NULL);
    glfwMakeContextCurrent(window);
	// Error check if the window fails to create
	if (window == NULL)
	{
		glfwTerminate();
		throw "Failed to create GLFW window";
	}

    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    // glfwSetCursorPosCallback(window, mouse_callback);
    // glfwSetScrollCallback(window, scroll_callback);
    // glfwSetMouseButtonCallback(window, mouse_button_callback);
    // Load GLAD so it configures OpenGL
	int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw "Failed to initialize OpenGL context";
    }
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, window_width, window_height);
    // Enables the Depth Buffer
	glEnable(GL_DEPTH_TEST);
    // Init player shader
    playerShader = new Shader("resources/shaders/default.vert", "resources/shaders/default.frag");
    
    player = new Player(playerShader);
}

// Start the client game loop.
void Client::loop(){
    // Main game loop
	while (!glfwWindowShouldClose(window))
	{
        // Render the client
        // world();
        render();

		// Take care of all GLFW events
		glfwPollEvents();
	}
    socket_active = false;
}

void Client::renderPlayers(){
    mtx.lock();
    std::unordered_map<boost::uuids::uuid, Player *> localOtherPlayers = otherPlayers;
    mtx.unlock();
    for (auto& x : localOtherPlayers){
        // If player is not initialized, initialize it.
        if (x.second->isInitialised() == false){
            x.second->init();
            x.second->setInitialised(true);
        }
        x.second->draw(window_width, window_height);
    }
}

void Client::sendPlayerInfo(){
    while (socket_active.load()){
        Package package(PLAYER_INFO, clientID, player->position);
        network.sendToServer(package);
    }
}

void Client::receivePlayersInfo(){
    while (socket_active.load()){
        Package package = network.receiveFromServer();
        if (package.clientID == boost::uuids::nil_uuid()){
            continue;
        }
        mtx.lock();
        if (package.clientID != clientID && otherPlayers.find(package.clientID) != otherPlayers.end()){
            // If the package is a disconnect package, remove the player from the map.
            if (package.type == PLAYER_DISCONNECT){
                std::cout << "Player disconnected" << std::endl;
                otherPlayers.erase(package.clientID);
                mtx.unlock();
                continue;
            }
            otherPlayers[package.clientID]->position = package.position;
        }
        else{
            // If the package is a disconnect package and the player is not previously loaded by the client, ignore the package.
            if (package.clientID == clientID || package.type == PLAYER_DISCONNECT){
                mtx.unlock();
                continue;
            }
            // If the player is not in the map, add the player to the map but do not initialize it, as it has to be done on the main thread (main loop()).
            otherPlayers[package.clientID] = new Player(playerShader, playerInfo(package.position));
        }
        mtx.unlock();
    }
}

void Client::world(){

}

// Render the client.
void Client::render() {
    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    player->keyboard_input(window);
    player->draw(window_width, window_height);

    renderPlayers();

    glfwSwapBuffers(window);
}

// Mouse callback functions for the client.
void Client::mouse_callback(GLFWwindow* window, double xpos, double ypos){

}

// Scroll callback function for the client.
void Client::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){

}

void Client::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

}

// Mouse button callback function for the client.
void Client::mouse_button_callback(GLFWwindow* window, int button, int action, int mode){

}

// Clean up the allocated files.
void Client::cleanup(){
    // ORDER OF DELETION IS IMPORTANT:

    // Delete player object
    delete player;
    // Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
}