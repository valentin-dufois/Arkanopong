#include "../includes/game.h"

/** Init gameObj with default values **/
void initGame()
{
    gameObj.wWidth = 1200;
    gameObj.wHeight = 800;

    strcpy(gameObj.theme, "themes/default/");

    gameObj.defVal.plateforme.size = 100;
    gameObj.defVal.plateforme.height = 15;
    gameObj.defVal.plateforme.maxSpeed = 25;
    gameObj.defVal.plateforme.AIMaxSpeed = 15;
    gameObj.defVal.plateforme.acceleration = .5;
    gameObj.defVal.plateforme.level = 35;
    gameObj.defVal.plateforme.narrowSize = 50;
    gameObj.defVal.plateforme.wideSize = 150;

    gameObj.defVal.brick.startLevel = 40;
    gameObj.defVal.brick.height = 20;
    
    gameObj.defVal.ball.size = 15;
    gameObj.defVal.ball.bigSize = 25;
    gameObj.defVal.ball.smallSize = 5;
    gameObj.defVal.ball.minSpeed = 5; 
    gameObj.defVal.ball.maxSpeed = 20;
    gameObj.defVal.ball.maxAngle = 90;
    gameObj.defVal.ball.gluedMaxDuration = 10 * 1000; /*ms*/

    gameObj.defVal.bonus.size = 15;
    gameObj.defVal.bonus.speed = 2;
    gameObj.defVal.bonus.duration = 10 * 1000; /*ms*/

    gameObj.defVal.wall.height = 35;

    gameObj.defVal.lifeNbr = 3;

    gameObj.gameState = MAINMENU;

    gameObj.textures = NULL;
    gameObj.nbrTextures = 0;

    gameObj.toPrint = NULL;
    gameObj.nbrToPrint = 0;
    gameObj.printContent = EMPTY;

    gameObj.keys.up = false;
    gameObj.keys.down = false;
    gameObj.keys.left = false;
    gameObj.keys.right = false;
    gameObj.keys.enter = false;
    gameObj.keys.esc = false;
    gameObj.keys.a = false;
    gameObj.keys.z = false;
    gameObj.keys.e = false;
    gameObj.keys.b = false;
    gameObj.keys.n = false;
    gameObj.keys.v = false;

    gameObj.currentlySelectedBtn = NULL;

    gameObj.game.nbrPlayers = 0;
    gameObj.game.players = NULL;

    gameObj.game.guidelines = false;

    gameObj.defautlTextColor = vec3(1, 1, 1);
    gameObj.selectedTextColor = vec3(0, 61.0/255.0, 81.0/255.0);

    srand(time(NULL));   /*Init rand*/
}

/*Déclaration de l'objet conteneur de propriétés du son*/
SDL_AudioSpec audioSortie;
Uint32 audioLen, audioPos;

SDL_AudioSpec audioBufferSpec;
    Uint8 *audioBuffer;
    Uint32 startTime, elapsedTime, audioBufferLen;

void audioCallback(void *udata, Uint8 *stream, int len)
{
    /* On ne lit que s'il reste des données à jouer */
    if ( audioLen == 0 )
        return;

    /* Remise à zéro du tampon de sortie */
    memset(stream, 0, len);

    /* Lecture du buffer audio */
    if (audioPos < audioBufferSpec.size) {
        if (audioPos+len > audioBufferSpec.size)
            len = audioBufferSpec.size = audioPos;
        SDL_MixAudio(stream, audioBuffer + audioPos,
            len, SDL_MIX_MAXVOLUME);
        audioPos += len;
    }

    /* Décrémentation de ce qu'il reste à lire */
    audioLen -= len;
}

/*Définition des propriétés du son*/
void audio_init()
{
    audioSortie.freq = 48000;
    audioSortie.format = AUDIO_S16;
    audioSortie.channels = 1;
    audioSortie.samples = 1024;
    audioSortie.callback = audioCallback;
    audioSortie.userdata = NULL;

    /*Initialisation de la couche audio*/
    if (SDL_OpenAudio(&audioSortie, NULL) < 0)
    {
        fprintf(stderr, "Erreur d'ouverture audio: %s\n", SDL_GetError());
    }
}

/** Main loop of the app**/
void theLoop()
{
    
    /*Loading of the sound in the buffer*/
    if(!SDL_LoadWAV("./themes/default/sounds/ok.wav", &audioBufferSpec,
    &audioBuffer, &audioBufferLen))
    {
        printf("Erreur lors du chargement du fichier WAV.\n");
    }

    while(gameObj.gameState != EXITING) 
    {
        startTime = SDL_GetTicks();
        glClear(GL_COLOR_BUFFER_BIT);

        doThings();

        printScreen();

        SDL_GL_SwapBuffers();

        watcher();
        
        elapsedTime = SDL_GetTicks() - startTime;
        if(elapsedTime < FRAMERATE_MILLISECONDS) 
        {
            SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
        }
    }
    
    /** TODO : Clean up app **/
    cleanToPrint();
    freeTextures();
    SDL_FreeWAV(audioBuffer);
}

/** Call current game state function **/
void doThings()
{
    switch(gameObj.gameState)
    {
        case MAINMENU:        mainMenu();        break;
        case THEMESELECTION:  themeSelection();  break;
        case PLAYERSELECTION: playerSelection(); break;
        case LEVELSELECTION:  playerSelection(); break;
        case STARTGAME:       startGame();       break;
        case INGAME:          ingame();          break;
        case ENDGAME:         endgame();         break;
        default: return;
    }
}

/** Handle the main menu **/
void mainMenu()
{
    char callback;

    if(gameObj.printContent != MAINMENU)
    {
        gameObj.defaultFont = loadFont("8BIT.ttf", 40);
        cleanToPrint();
        createMainMenu();
        return;
    }

    callback = btnHandler();

    /*Do something with the callback*/
    if(callback == 's')
    {
        /*jouer son ici*/
        SDL_PauseAudio(0);

        while ( audioLen > 0 )
            SDL_Delay(100);

        SDL_CloseAudio();

        gameObj.gameState = PLAYERSELECTION;
    }

    if(callback == 't')
    {
        gameObj.gameState = THEMESELECTION;
    }

    if(callback == 'q')
    {
        gameObj.gameState = EXITING;
    }
}



/** Theme switch screen **/
void themeSelection()
{
    char callback;

    if(gameObj.printContent != THEMESELECTION)
    {
        cleanToPrint();
        createThemeSelection();
    }

    callback = btnHandler();

    if(callback == 'b')
    {
        gameObj.gameState = MAINMENU;
        return;
    }
    
    if(callback != 0)
    {
        /*Apply new theme*/
        setTheme(callback);
        gameObj.gameState = MAINMENU;
    }
}


/** Handle nbr of player selection **/
void playerSelection()
{
    char callback;

    if(gameObj.printContent != PLAYERSELECTION)
    {
        cleanToPrint();
        createPlayerSelection();
        return;
    }

    callback = btnHandler();

    if(callback == 'b')
    {
        gameObj.gameState = MAINMENU;
        return;
    }
    
    if(callback == 'p')
    {
        gameObj.game.humans = gameObj.playerSelection.humans->value;
        gameObj.game.computers = gameObj.playerSelection.computers->value;

        gameObj.gameState = STARTGAME;
    }
}

/** Start the game **/
void startGame()
{
    int i, p;
    char pos[11][7];
    /*bool addAI = false;*/

    if(gameObj.game.players != NULL)
    {
        /*Free players*/
    }
    
    /*Make sure there is at least two players*/
    if(gameObj.game.humans == 1 && gameObj.game.computers == 0)
    {
        gameObj.game.computers = 1;
    }

    gameObj.game.nbrPlayers = gameObj.game.humans + gameObj.game.computers;

    /*Board layout for all the different configurations*/

    /*  HA     -     HA  */ pos[0][0] = 'H'; pos[0][1] = 'A';
    /*  HAA    -    HAA  */ pos[1][0] = 'H'; pos[1][1] = 'A'; pos[1][2] = 'A'; 
    /*  HAAA   -   HAAA  */ pos[2][0] = 'H'; pos[2][1] = 'A'; pos[2][2] = 'A'; pos[2][3] = 'A'; 
    /*  HH     -     HH  */ pos[3][0] = 'H'; pos[3][1] = 'H'; 
    /*  HHA    -    AHH  */ pos[4][0] = 'A'; pos[4][1] = 'H'; pos[4][2] = 'H'; 
    /*  HHAA   -   HAHA  */ pos[5][0] = 'H'; pos[5][1] = 'A'; pos[5][2] = 'H'; pos[5][3] = 'A'; 
    /*  HHAAA  -  AHAAH  */ pos[6][0] = 'A'; pos[6][1] = 'H'; pos[6][2] = 'A'; pos[6][3] = 'A'; pos[6][4] = 'H'; 
    /*  HHH    -    HHH  */ pos[7][0] = 'H'; pos[7][1] = 'H'; pos[7][2] = 'H'; 
    /*  HHHA   -   HHAH  */ pos[8][0] = 'H'; pos[8][1] = 'H'; pos[8][2] = 'A'; pos[8][3] = 'H'; 
    /*  HHHAA  -  HAHHA  */ pos[9][0] = 'H'; pos[9][1] = 'A'; pos[9][2] = 'H'; pos[9][3] = 'H'; pos[9][4] = 'A'; 
    /*  HHHAAA - HAHAHA  */ pos[10][0] = 'H'; pos[10][1] = 'A'; pos[10][2] = 'H'; pos[10][3] = 'A'; pos[10][4] = 'H'; pos[10][5] = 'A'; 

    if(gameObj.game.humans == 1 && gameObj.game.computers == 1)
        p = 0;
    else if(gameObj.game.humans == 1 && gameObj.game.computers == 2)
        p = 1;
    else if(gameObj.game.humans == 1 && gameObj.game.computers == 3)
        p = 2;
    else if(gameObj.game.humans == 2 && gameObj.game.computers == 0)
        p = 3;
    else if(gameObj.game.humans == 2 && gameObj.game.computers == 1)
        p = 4;
    else if(gameObj.game.humans == 2 && gameObj.game.computers == 2)
        p = 5;
    else if(gameObj.game.humans == 2 && gameObj.game.computers == 3)
        p = 6;
    else if(gameObj.game.humans == 3 && gameObj.game.computers == 0)
        p = 7;
    else if(gameObj.game.humans == 3 && gameObj.game.computers == 1)
        p = 8;
    else if(gameObj.game.humans == 3 && gameObj.game.computers == 2)
        p = 9;
    else /*if(gameObj.game.humans == 3 && gameObj.game.computers == 3)*/
        p = 10;


    /*Create bounding box*/
    defineBoundingBox();
    gameObj.game.players = allocate(sizeof(Player *) * gameObj.game.nbrPlayers);

    gameObj.game.humans = 0;

    for(i = 0; i < gameObj.game.nbrPlayers; ++i)
    {
        if(pos[p][i] == 'H')
        {
            /*Insert Human*/
            createPlayer(HUMAN, i);
            gameObj.game.players[i]->controls = gameObj.game.humans;
            gameObj.game.humans++;

            if(bboxIsReversed(i))
            {
                gameObj.game.players[i]->reversed = true;
            }
        }
        else
        {
            /*Insert computer*/
            createPlayer(AI, i);
        }
    }

    gameObj.gameState = INGAME;
}

/** Create a new player **/
void createPlayer(enum PlayerType type, int playerNbr)
{   
    /*Define plateforme position*/
    int platX = 0 - gameObj.defVal.plateforme.size / 2;
    int platY = gameObj.game.bb.height - gameObj.defVal.plateforme.level;

    /*Create the player*/
    Player * player = allocate(sizeof(Player));
    gameObj.game.players[playerNbr] = player;

    player->type = type;
    player->life = gameObj.defVal.lifeNbr;

    /*Create the player's plateforme*/
    player->plateforme = createPlateforme(platX, platY);
    player->plateforme->BBox = playerNbr;

    player->reversed = false;
}

/** Handle ingame events **/
void ingame()
{
    if(gameObj.printContent != INGAME)
    {
        cleanToPrint();
        createGameBoard();
        return;
    }

    playerMovements();

    ballMovements();

    bonusMovements();
}

void endgame()
{
    static bool displayed = false;
    int i;
    char caption[50];

    if(gameObj.printContent != ENDGAME)
    {
        addToPrint(createText("FIN", 0, 0, gameObj.defaultFont), TEXT);

        gameObj.printContent = ENDGAME;
        return;
    }

    SDL_Delay(2000);

    if(!displayed)
    {  
        displayed = true;

        if(gameObj.game.computers > 0)
        {
            strcpy(caption, "L ordinateur remporte la partie");
            addToPrint(createText(caption, 0, -60, gameObj.defaultFont), TEXT);
            return;
        }

        for(i = 0; i < gameObj.game.nbrPlayers; ++i)
        {
            if(gameObj.game.players[i]->life != 0)
            {
                strcpy(caption, "Joueur ");
                strcat(caption, itoa(i+1));
                strcat(caption, " gagne la partie");
                addToPrint(createText(caption, 0, -60, gameObj.defaultFont), TEXT);
            }
        }

        return;
    }

    SDL_Delay(2000);

    gameObj.gameState = PLAYERSELECTION;
}