#include "../includes/game.h"

void printScreen()
{
    int nbrToPrint = gameObj.nbrToPrint;
    int i;
    PrintElement el;

    /*No need to go further if there's nothing to print*/
    if(gameObj.printContent == EMPTY || gameObj.nbrToPrint == 0)
        return;

    /** Clean screen ?**/                            
    
    /*Iterate over all the element of the toPrint array*/
    for(i = 0; i < nbrToPrint; i++)             
    {
        el = gameObj.toPrint[i];

        switch(el.type)          
        {
            case PLATEFORME: printPlateforme(el.element.plateforme); break;
            case PICTURE: printPicture(el.element.pict); break;
            case BRICK: printPicture(el.element.pict); break;
            case BALL: printPicture(el.element.pict); break;
            case BONUS: printPicture(el.element.pict); break;
            case BUTTON: printButton(el.element.btn); break;
        }
    }
}

PrintElement * addToPrint(void * element, enum elType type)
{
    int newSize;
    PrintElement newEl;
    
    /*We make sure no NULL pointer get's in the loop*/
    criticalIfNull(element);                    
    
    newEl.type = type;                             
    
    switch(type)          
    {
        case PLATEFORME: newEl.element.plateforme = element; break;
        case BRICK: newEl.element.brick = element; break;
        case BALL: newEl.element.ball = element; break;
        case BONUS: newEl.element.bonus = element; break;
        case BUTTON: newEl.element.btn = element; break;
        case PICTURE: newEl.element.pict = element; break;
    }
    
    /*Add room in the toPrint array to store this new element*/
    newSize = (gameObj.nbrToPrint + 1) * sizeof(PrintElement);

    if(gameObj.nbrToPrint == 0)
        gameObj.toPrint = allocate(newSize);
    else
        gameObj.toPrint = reAllocate(gameObj.toPrint, newSize); 

    /*Insert newly created printElement in the toPrint array*/
    gameObj.toPrint[gameObj.nbrToPrint] = newEl;                 
    gameObj.nbrToPrint++;
    
    /*Return the newly created element so it can be edited*/
    return &gameObj.toPrint[gameObj.nbrToPrint-1];       
}

/**cleanPrint() empty the toPrint array**/
void cleanToPrint()
{
    int i;

    if(gameObj.nbrToPrint != 0)
    {
        for(i = 0; i < gameObj.nbrToPrint; i++)
        {
            /*Free the element with the appropriate function*/
            switch(gameObj.toPrint[i].type)          
            {
                case PLATEFORME: freePlateforme(gameObj.toPrint[i].element.plateforme); break;
                case BRICK: freePlateforme(gameObj.toPrint[i].element.plateforme); break;
                case BALL: freePlateforme(gameObj.toPrint[i].element.plateforme); break;
                case BONUS: freePlateforme(gameObj.toPrint[i].element.plateforme); break;
                case BUTTON: freeButton(gameObj.toPrint[i].element.btn); break;
                case PICTURE: freePicture(gameObj.toPrint[i].element.pict); break;
            }
        }

        free(gameObj.toPrint); 
        
        gameObj.toPrint = NULL;
        gameObj.nbrToPrint = 0;                                     
    }
}



void freeTextures()
{
    int i;

    /*Free all textures*/
    glDeleteTextures(gameObj.nbrTextures, gameObj.textures);

    for(i = 0; i < gameObj.nbrTextures; ++i)
    {
        free(gameObj.texturesPath[i]);
    }

    free(gameObj.texturesPath);
    
    gameObj.texturesPath = NULL;
    gameObj.nbrTextures = 0;
}