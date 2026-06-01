#ifndef TEXT_FIELD_H
#define TEXT_FIELD_H

#include <SFML/Graphics.h>

#include "frontend/ui/ui_elements.h"
#include "input/mouse.h"

typedef enum TextFieldState
{
    textFieldIdle,
    textFieldHovered,
    textFieldSelected

} TextFieldState;


/* Lifecycle */
TextField* textField_create(void);
TextField* textField_copy(const TextField*);
void textField_destroy(TextField*);

/* General manipulation */
void textField_setSize(TextField*, sfVector2f);
sfVector2f textField_getSize(const TextField*);

void textField_setPosition(TextField*, sfVector2f);
sfVector2f textField_getPosition(const TextField*);

void textField_setOrigin(TextField*, sfVector2f);
sfVector2f textField_getOrigin(const TextField*);

/* Fill color manipulation */
void textField_setFillColor(TextField*, sfColor);
sfColor textField_getFillColor(const TextField*);

/* Text manipulation */
void textField_setTextString(TextField*, const char*);
const char* textField_getTextString(const TextField*);

void textField_clear(TextField*);

void textField_setTextFont(TextField*, const sfFont*);
const sfFont* textField_getTextFont(const TextField*);

void textField_setCharacterSize(TextField*, unsigned int);
unsigned int textField_getCharacterSize(const TextField*);

void textField_setLetterSpacing(TextField*, float);
float textField_getLetterSpacing(const TextField*);

void textField_setTextColor(TextField*, sfColor);
sfColor textField_getTextColor(const TextField*);

void textField_setTextPadding(TextField*, sfVector2f);
sfVector2f textField_getTextPadding(const TextField*);

/* Texture manipulation */
void textField_setTexture(TextField*, const sfTexture*, sfBool);
const sfTexture* textField_getTexture(const TextField*);

void textField_setTextureRect_onIdle(TextField*, sfIntRect);
sfIntRect textField_getTextureRect_onIdle(const TextField*);

void textField_setTextureRect_onHover(TextField*, sfIntRect);
sfIntRect textField_getTextureRect_onHover(const TextField*);

void textField_setTextureRect_onSelected(TextField*, sfIntRect);
sfIntRect textField_getTextureRect_onSelected(const TextField*);

/* Outline manipulation */
void textField_setOutlineColor(TextField*, sfColor);
sfColor textField_getOutlineColor(const TextField*);

void textField_setOutlineThickness(TextField*, float);
float textField_getOutlineThickness(const TextField*);

void textField_setSelectedOutlineColor(TextField*, sfColor);
sfColor textField_getSelectedOutlineColor(const TextField*);

void textField_setSelectedOutlineThickness(TextField*, float);
float textField_getSelectedOutlineThickness(const TextField*);

/* State and input */
TextFieldState textField_getState(const TextField*);
sfBool textField_isSelected(const TextField*);

void textField_select(TextField*);
void textField_deselect(TextField*);

void textField_updateMouse(TextField*, const Mouse*);
void textField_updateKeyboard(TextField*, const sfEvent*);

/* Utility */
sfBool textField_containsPoint(const TextField*, sfVector2i);
sfFloatRect textField_getGlobalBounds(const TextField*);
void textField_draw(sfRenderWindow*, const TextField*);

#endif // TEXT_FIELD_H