#ifndef BUTTON_H
#define BUTTON_H

#include <SFML/Graphics.h>

#include "frontend/ui/ui_elements.h"
#include "input/mouse.h"

typedef void(*buttonAction)(void*);

typedef enum ButtonState
{
    buttonIdle,
    buttonHovered,
    buttonPressed

} ButtonState;


/* Lifecycle */
Button* button_create(void);
Button* button_copy(const Button*);
void button_destroy(Button*);

/* General manipulation */
void button_setSize(Button*, sfVector2f);
sfVector2f button_getSize(const Button*);

void button_setPosition(Button*, sfVector2f);
sfVector2f button_getPosition(const Button*);

void button_setOrigin(Button*, sfVector2f);
sfVector2f button_getOrigin(const Button*);

/* Fill color manipulation */
void button_setFillColor(Button*, sfColor);
sfColor button_getFillColor(const Button*);

/* Text manipulation */
void button_setTextString(Button*, const char*);
const char* button_getTextString(const Button*);

void button_setTextFont(Button*, const sfFont*);
const sfFont* button_getTextFont(const Button*);

void button_setCharacterSize(Button*, unsigned int);
unsigned int button_getCharacterSize(const Button*);

void button_setLetterSpacing(Button*, float);
float button_getLetterSpacing(const Button*);

void button_setTextColor(Button*, sfColor);
sfColor button_getTextColor(const Button*);

/* Texture manipulation */
void button_setTexture(Button*, const sfTexture*, sfBool);
const sfTexture* button_getTexture(const Button*);

void button_setTextureRect_onIdle(Button*, sfIntRect);
sfIntRect button_getTextureRect_onIdle(const Button*);

void button_setTextureRect_onHover(Button*, sfIntRect);
sfIntRect button_getTextureRect_onHover(const Button*);

void button_setTextureRect_onPress(Button*, sfIntRect);
sfIntRect button_getTextureRect_onPress(const Button*);

/* Outline manipulation */
void button_setOutlineColor(Button*, sfColor);
sfColor button_getOutlineColor(const Button*);

void button_setOutlineThickness(Button*, float);
float button_getOutlineThickness(const Button*);

/* State and input */
ButtonState button_getState(const Button*);

void button_setAction(Button*, buttonAction, void*);
buttonAction button_getAction(const Button*);
void* button_getActionData(const Button*);
void button_triggerAction(Button*);

void button_updateMouse(Button*, const Mouse*);

/* Utility */
sfBool button_containsPoint(const Button*, sfVector2i);
sfFloatRect button_getGlobalBounds(const Button*);
void button_draw(sfRenderWindow*, const Button*);

#endif // BUTTON_H